/**
 * @brief   16550 UART implementation
 * 
 * @copyright
 *  Copyright (C) 2024 Seb Atkinson\n
 *  Copyright (C) 2023-2024 John Jekel\n
 *  See the LICENSE file at the root of the project for licensing info.
 * 
 * TODO longer description
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
 * Constants/Defines
 * --------------------------------------------------------------------------------------------- */

//TODO

/* ------------------------------------------------------------------------------------------------
 * Type/Class Declarations
 * --------------------------------------------------------------------------------------------- */

//TODO

/* ------------------------------------------------------------------------------------------------
 * Static Variables
 * --------------------------------------------------------------------------------------------- */

//TODO

/* ------------------------------------------------------------------------------------------------
 * Static Function Declarations
 * --------------------------------------------------------------------------------------------- */

//TODO

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
    //irvelog_always(0, "Reading from UART register 0x%02X", static_cast<uint8_t>(register_address));
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
                    assert(false && "Tried to read from empty input");//Buggy software tried to read form empty queue.
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
            this->m_isr_read_since_last_thr_write = true;
            return this->construct_isr();
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
                return this->regs.m_psd;
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
    //irvelog_always(0, "Writing 0x%02X to UART register 0x%02X", data, static_cast<uint8_t>(register_address));
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
            assert(((data & 0b11) == 0b11) && "Only 8 bit characters are supported!");
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

void Uart::update_receive() {
    uint8_t data;
    ssize_t bytesRead = ::read(this->receive_file_fd, &data, 1);
    if(bytesRead > 0){
            receive_queue.push(data);
    }
}

bool Uart::interrupt_pending() {
    update_receive();
    return (this->construct_isr() & 0b1) == 0;
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

uint8_t Uart::construct_isr() const {
    uint8_t isr = 0;
    isr |= (this->regs.m_fcr & 0b1) ? (0b11 << 6) : 0;//Indicate the FIFOs are enabled if they infact are

    //If there is a character available to read, and the Received Data Ready interrupt is enabled
    if ((receive_queue.size() > 0) && (this->regs.m_ier & 0b1)) {
        isr |= 0b0100;
    //If the THR empty interrupt is enabled (since we are always ready to transmit) and...
    //the user hasn't already read the ISR to check this
    } else if ((this->regs.m_ier & 0b10) && !this->m_isr_read_since_last_thr_write) {
        isr |= 0b0010;
    } else {//No enabled interrupts pending
        isr |= 0b0001;
    }//We don't need to support any other interrupt types

    return isr;
}
