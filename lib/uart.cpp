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

#include "uart.h"
#include "tsqueue.h"

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
    transmit_thread = std::thread(&Uart::transmit_thread_function, this);
    receive_thread = std::thread(&Uart::receive_thread_function, this);
    //TODO
}

Uart::~Uart() {
    if (this->m_output_line_buffer.size() > 0) {
        irvelog_always_stdout(
            0,
            "\x1b[92mRVUARTTX:\x1b[0m: \"\x1b[1m%s\x1b[0m\"",
            this->m_output_line_buffer.c_str()
        );
    }
    {                                  
        std::lock_guard<std::mutex> lock(this->transmit_mutex); 
        this->kill_transmit_thread = true; 
        this->transmit_condition_variable.notify_one();          
    }
    {                                  
        std::lock_guard<std::mutex> lock(this->receive_mutex); 
        this->kill_receive_thread = true;      
    }
    if(transmit_thread.joinable()){
        transmit_thread.join();
    }
    if(receive_thread.joinable()){
        std::cout<<"Press Enter to kill UART"<<std::endl;
        receive_thread.join();
    }
}

uint8_t Uart::read(Uart::Address register_address) {
    assert((static_cast<uint8_t>(register_address) <= 0b111) && "Invalid UART register address!");
    switch (register_address) {
        case Uart::Address::RHR: {//RHR or DLL (Never THR since that isn't readable)
            if (this->dlab()) {//DLL
                return this->m_dll;
            } else {//RHR
                char read_data;
                if(this->async_receive_queue.size() == 0){
                    assert(false && "No data to read from UART");//TODO
                }
                if(this->async_receive_queue.size() == 1){
                    {
                        std::lock_guard<std::mutex> lock(this->receive_mutex);
                        this->m_lsr &= 0b11111110;//Clear data ready bit to HIGH
                        read_data = char(this->async_receive_queue.front());
                        this->async_receive_queue.pop();
                    }
                }else{
                    {
                        std::lock_guard<std::mutex> lock(this->receive_mutex);
                        this->m_lsr &= 0b11111110;//Clear data ready bit to HIGH
                        read_data = char(this->async_receive_queue.front());
                        this->async_receive_queue.pop();
                    }
                }
                return read_data;
                //For now, this is a call to std::getchar, but will eventually be a 
                //read from a fd, which is a virtual UART device on the host computer.
            }
            break;
        }
        case Uart::Address::IER: {//IER or DLM
            if (this->dlab()) {//DLM
                return this->m_dlm;
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
            uint8_t LSR_val;//Ensures no race conditions
            {
                std::lock_guard<std::mutex> lock(this->receive_mutex);
                LSR_val = this->m_lsr;
            }
            return LSR_val | (1 << 5);//We are always ready to transmit
        }
        case Uart::Address::MSR: {
            assert(false && "TODO");//TODO
            break;
        }
        case Uart::Address::SPR: {
            return this->m_spr;
            break;
        }

        default: assert(false && "We should never get here!"); return 0;
    }
}

void Uart::write(Uart::Address register_address, uint8_t data) {
    assert((static_cast<uint8_t>(register_address) <= 0b111) && "Invalid UART register address!");
    switch (register_address) {
        case Uart::Address::THR: {//THR or DLL (Never RHR since that isn't writable)
            if (this->dlab()) {//DLL
                this->m_dll = data;
            } else {//THR
                {//Wake up the transmit thread                          
                    std::lock_guard<std::mutex> lock(this->transmit_mutex); 
                    this->transmit_condition_variable.notify_one();          
                }       
                this->async_transmit_queue.push(data);
            }
            break;
        }
        case Uart::Address::IER: {//IER or DLM
            if (this->dlab()) {//DLM
                this->m_dlm = data;
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
            this->m_psd = data;
            break;
        case Uart::Address::MSR://NOTE: Should never be written to by software
            assert(false && "TODO software was mean");//TODO
            break;
        case Uart::Address::SPR: {
            this->m_spr = data;
            break;
        }
        default: assert(false && "We should never get here!");
    }
}

bool Uart::interrupt_pending() const {
    return this->m_isr & 0b1;
}

bool Uart::dlab() const {
    return this->m_lcr & (1 << 7);
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

void Uart::receive_thread_function(){
    //This wile pole the input and push any data transmitted to the read fifo.
    //Once pushed, the main thread can pop from the fifo.
    while (!this->kill_receive_thread) {
        char newChar;
        newChar = std::getchar();
        {
            std::lock_guard<std::mutex> lock(this->receive_mutex);
            this->m_lsr |= 0b00000001;//Set data ready bit to HIGH
        }
        this->async_receive_queue.push(newChar);
    }
}
