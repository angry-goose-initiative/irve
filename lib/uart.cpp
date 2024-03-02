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

Uart::Uart() {
    this->regs = {static_cast<uint8_t>(irve_fuzzish_rand()), static_cast<uint8_t>(irve_fuzzish_rand()), static_cast<uint8_t>(irve_fuzzish_rand()), static_cast<uint8_t>(irve_fuzzish_rand()), static_cast<uint8_t>(irve_fuzzish_rand()), static_cast<uint8_t>(irve_fuzzish_rand()), static_cast<uint8_t>(irve_fuzzish_rand())};
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
                    assert(false && "Tried to read from empty input");//Buggy software tried to read form empty queue.
                }
                return data;
            }
            break;
        }
        case Uart::Address::IER: {//IER or DLM
            if (this->dlab()) {//DLM
                return this->regs.m_dlm;
            } else {//IER
                assert(false && "TODO");//TODO
            }
            break;
        }
        case Uart::Address::ISR: {//NOTE: Never FCR since that isn't readable
            assert(false && "TODO");//TODO
            break;
        }
        case Uart::Address::LCR: {
            assert(false && "TODO");//TODO
            break;
        }
        case Uart::Address::MCR: {
            assert(false && "TODO");//TODO
            break;
        }
        case Uart::Address::LSR: {
            //Clear the Data Ready bit in the LSR if the queue is empty
            if (receive_queue.size() == 0) {
                constexpr uint32_t LSR_DATA_READY_POS = 0U;
                this->regs.m_lsr &= ~(1U << LSR_DATA_READY_POS);
                
                //TODO do we need to clear the interrupt pending bit too?
            }

            constexpr uint32_t LSR_TX_READY_POS = 5U;
            return this->regs.m_lsr | (1U << LSR_TX_READY_POS);//We are always ready to transmit
        }
        case Uart::Address::MSR: {
            assert(false && "TODO");//TODO
            break;
        }
        case Uart::Address::SPR: {
            return this->regs.m_spr;
            break;
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
                assert(false && "TODO");//TODO
            }
            break;
        }
        case Uart::Address::FCR: {//NOTE: Never ISR since that isn't writable
            assert(false && "TODO");//TODO
            break;
        }
        case Uart::Address::LCR: {
            assert(false && "TODO");//TODO
            break;
        }
        case Uart::Address::MCR: {
            assert(false && "TODO");//TODO
            break;
        }
        case Uart::Address::PSD://NOTE: Never LSR since that isn't writable
            assert(this->dlab() && "TODO software was mean");//TODO
            this->regs.m_psd = data;
            break;
        case Uart::Address::MSR://NOTE: Should never be written to by software
            assert(false && "TODO software was mean");//TODO
            break;
        case Uart::Address::SPR: {
            this->regs.m_spr = data;
            break;
        }
        default: assert(false && "We should never get here!");
    }
}

void Uart::update_receive() {
    uint8_t data;
    ssize_t bytesRead = ::read(this->receive_file_fd, &data, 1);
    if(bytesRead > 0){
            receive_queue.push(data);
            this->regs.m_isr |= 0b1;
            this->regs.m_lsr |= 0b1;
    }else if(receive_queue.size() == 0){
        this->regs.m_isr &= ~0b1;
        this->regs.m_lsr &= ~0b1;
    }
}

bool Uart::interrupt_pending() {
    update_receive();
    return this->regs.m_isr & 0b1;
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
