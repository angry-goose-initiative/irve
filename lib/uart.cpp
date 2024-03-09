/**
 * @brief   8250/16550 UART implementation
 * 
 * @copyright
 *  Copyright (C) 2024 Seb Atkinson\n
 *  Copyright (C) 2023-2024 John Jekel\n
 *  See the LICENSE file at the root of the project for licensing info.
 * 
 * Based on specification described in http://caro.su/msx/ocm_de1/16550.pdf
 *
*/

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include <cassert>
#include <cstdint>
#include <string>
#include <cstdio>
#include <thread>
#include <iostream>
#include <condition_variable>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

#include "common.h"

#include "uart.h"
#include "tsqueue.h"
#include "fuzzish.h"

#define INST_COUNT 0
#include "logging.h"

using namespace irve::internal;

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

Uart::Uart() :
    m_isr_read_since_last_thr_write(true)
{
    this->regs = {
        //Reset values for the UART registers per the 16550 datasheet
        .m_ier = 0x00,
        .m_fcr = 0x00,
        .m_lcr = 0x03,//Non-standard reset value since we only support 8 bit characters
        .m_mcr = 0x00,
        .m_spr = 0x00,
        .m_dll = static_cast<uint8_t>(irve_fuzzish_rand()),
        .m_dlm = static_cast<uint8_t>(irve_fuzzish_rand()),
        .m_psd = 0x00
    };
    this->transmit_thread = std::thread(&Uart::transmit_thread_function, this);
    this->receive_file_fd = fileno(stdin);
    int flags = fcntl(this->receive_file_fd, F_GETFL, 0);
    fcntl(this->receive_file_fd, F_SETFL, flags | O_NONBLOCK);

    //Save original terminal settings
    this->m_original_receive_file_fd_settings = termios();
    tcgetattr(this->receive_file_fd, &this->m_original_receive_file_fd_settings);

    //Disable buffering characters until \n is entered, and echo
    struct termios new_receive_file_fd_settings = this->m_original_receive_file_fd_settings;
    new_receive_file_fd_settings.c_lflag &= ~ICANON;
    new_receive_file_fd_settings.c_lflag &= ~ECHO;
    tcsetattr(this->receive_file_fd, TCSANOW, &new_receive_file_fd_settings);
}

Uart::~Uart() {
    //Restore terminal settings
    tcsetattr(this->receive_file_fd, TCSANOW, &this->m_original_receive_file_fd_settings);

    {                                  
        std::lock_guard<std::mutex> lock(this->transmit_mutex); 
        this->kill_transmit_thread = true; 
        this->transmit_condition_variable.notify_one();          
    }
    if(transmit_thread.joinable()){
        transmit_thread.join();
    }
}

uint8_t Uart::read(Uart::Address register_address) {
    assert((static_cast<uint8_t>(register_address) <= 0b111) && "Invalid UART register address!");
    switch (register_address) {
        case Uart::Address::RHR: {//RHR or DLL (Never THR since that isn't readable)
            if (this->dlab()) {//DLL
                return this->regs.m_dll;
            } else {//RHR
                uint8_t data = 0;
                if(receive_queue.size()>0){
                    data = receive_queue.front();
                    receive_queue.pop();
                }else{
                    irvelog(0, "Software tried to read from the RHR even though it's empty!");
                }
                return data;
            }
        }
        case Uart::Address::IER: {//IER or DLM
            if (this->dlab()) {//DLM
                return this->regs.m_dlm;
            } else {//IER
                return this->regs.m_ier;
            }
        }
        case Uart::Address::ISR: {//NOTE: Never FCR since that isn't readable
            uint8_t isr = this->construct_isr();//Do this first...
            this->m_isr_read_since_last_thr_write = true;//Since this changes the value of the ISR
            return isr;
        }
        case Uart::Address::LCR: {
            return this->regs.m_lcr;
        }
        case Uart::Address::MCR: {
            return this->regs.m_mcr;
        }
        case Uart::Address::LSR: {
            if (this->dlab()) {
                irvelog(0, "Software tried to read from the UART's PSD register, which is write only!");
                return 0;
            }
            //No need to implement any of the error bits, just the TX and RX ready bits
            constexpr uint32_t LSR_TX_NOT_IN_PROGRESS_POS   = 6U;
            constexpr uint32_t LSR_TX_READY_POS             = 5U;
            constexpr uint32_t LSR_RX_READY_POS             = 0U;
            uint8_t lsr = 0;
            lsr |= 1U << LSR_TX_NOT_IN_PROGRESS_POS;//We are always ready to transmit
            lsr |= 1U << LSR_TX_READY_POS;//We are always ready to transmit
            lsr |= (receive_queue.size() > 0) ? (1U << LSR_RX_READY_POS) : 0;//Set this if characters are waiting!
            return lsr;
        }
        case Uart::Address::MSR: {
            //This register just reports the values of the bunch of control signals we learned about in ECE 224
            //Making the lower four bits 0 indicates no changes have occured
            //Making the high four bits 0 indicates (since they are active low) that the CTS, DSR, RI and CD
            //lines are all high, which should please most software that uses this
            return 0;
        }
        case Uart::Address::SPR: {
            return this->regs.m_spr;
        }
    }

    assert(false && "We should never get here!");
    __builtin_unreachable();
}

void Uart::write(Uart::Address register_address, uint8_t data) {
    assert((static_cast<uint8_t>(register_address) <= 0b111) && "Invalid UART register address!");
    switch (register_address) {
        case Uart::Address::THR: {//THR or DLL (Never RHR since that isn't writable)
            if (this->dlab()) {//DLL
                this->regs.m_dll = data;
            } else {//THR
                this->m_isr_read_since_last_thr_write = false;
                this->async_transmit_queue.push(data);
                {//Wake up the transmit thread                          
                    std::lock_guard<std::mutex> lock(this->transmit_mutex); 
                    this->transmit_condition_variable.notify_one();          
                }       
            }
            break;
        }
        case Uart::Address::IER: {//IER or DLM
            if (this->dlab()) {//DLM
                this->regs.m_dlm = data;
            } else {//IER
                this->regs.m_ier = data & 0xF;//Only the lower 4 bits are used
            }
            break;
        }
        case Uart::Address::FCR: {//NOTE: Never ISR since that isn't writable
            this->regs.m_fcr = data & 0b11000111;
            break;
        }
        case Uart::Address::LCR: {
            if ((data & 0b11) == 0b11) {
                irvelog(0, "Software changed the character mode away from 8 bit characters, which is not supported!");
                irvelog(0, "Upper bits written to the THR will not be ignored, and these bits will still be provided via the RHR!");
            }
            //Note: We needn't do anything special for Set Break or the parity fields
            this->regs.m_lcr = data;
            break;
        }
        case Uart::Address::MCR: {
            this->regs.m_mcr = data & 0xF;
            [[maybe_unused]] constexpr uint32_t LOOPBACK_POS = 4U;
            if (data & (1U << LOOPBACK_POS)) {
                assert(false && "Loopback mode not implemented, but software tried to use it!");
            }
            break;
        }
        case Uart::Address::PSD://NOTE: Never LSR since that isn't writable
            if (this->dlab()) {
                this->regs.m_psd = data & 0x0F;
            } else {
                irvelog(0, "Software tried to write to the UART's LSR register, which is read only!");
            }
            break;
        case Uart::Address::MSR://NOTE: Should never be written to by software
            irvelog(0, "Software tried to write to the UART's MSR register, which is read only!");
            break;
        case Uart::Address::SPR: {
            this->regs.m_spr = data;
            break;
        }
        default: {
            assert(false && "We should never get here!"); 
            __builtin_unreachable();
        }
    }
}

uint8_t Uart::construct_isr() const {
    uint8_t isr = 0;
    constexpr uint32_t FIFO_ENABLED_FCR_POS = 0U;
    if (this->regs.m_fcr & (1U << FIFO_ENABLED_FCR_POS)) {
        constexpr uint32_t FIFO_ENABLED_ISR_VALUE   = 0b11;
        constexpr uint32_t FIFO_ENABLED_ISR_POS     = 6U;
        isr |= FIFO_ENABLED_ISR_VALUE << FIFO_ENABLED_ISR_POS;
    }

    //If there is a character available to read, and the Received Data Ready interrupt is enabled
    constexpr uint32_t DATA_READY_IER_POS   = 0U;
    constexpr uint32_t THR_EMPTY_IER_POS    = 1U;
    if ((receive_queue.size() > 0) && (this->regs.m_ier & (1U << DATA_READY_IER_POS))) {
        isr |= 0b0100;//Code indicating the Received Data Ready interrupt is pending
    //If the THR empty interrupt is enabled (since we are always ready to transmit) and...
    //the user hasn't already read the ISR to check this
    } else if ((this->regs.m_ier & (1U << THR_EMPTY_IER_POS)) && !this->m_isr_read_since_last_thr_write) {
        isr |= 0b0010;//Code indicating the THR empty interrupt is pending
    } else {//No enabled interrupts pending
        isr |= 0b0001;//Code indicating no interrupts are pending
    }//We don't need to support any other interrupt types

    return isr;
}

void Uart::update_receive() {
    uint8_t data;
    ssize_t bytesRead = ::read(this->receive_file_fd, &data, 1);
    if(bytesRead > 0){
            receive_queue.push(data);
    }
}

bool Uart::interrupt_pending() {
    update_receive();
    constexpr uint32_t INTERRUPT_STATUS_ISR_POS = 0U;
    return (this->construct_isr() & (1U << INTERRUPT_STATUS_ISR_POS)) == 0;
}

bool Uart::dlab() const {
    return this->regs.m_lcr & (1 << 7);
}

void Uart::transmit_thread_function(){
    std::unique_lock<std::mutex> lock(this->transmit_mutex); 
    while (!this->kill_transmit_thread){
        this->transmit_condition_variable.wait(lock);
        while(this->async_transmit_queue.size() > 0){
            char data = char(this->async_transmit_queue.front());
            this->async_transmit_queue.pop();
            std::cout<<data<<std::flush;
        }
    }
}
