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
    write_thread = std::thread(&Uart::write_thread_function, this);
    read_thread = std::thread(&Uart::read_thread_function, this);
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
    if (write_thread.joinable())
    {
        write_thread.join();
    }
    if (read_thread.joinable())
    {
        read_thread.join();
    }
}

uint8_t Uart::read(Uart::Address register_address) {
    assert((static_cast<uint8_t>(register_address) <= 0b111) && "Invalid UART register address!");
    switch (register_address) {
        case Uart::Address::RHR: {//RHR or DLL (Never THR since that isn't readable)
            if (this->dlab()) {//DLL
                return this->m_dll;
            } else {//RHR
                return std::getchar();
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
            return this->m_lsr;
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
                /*
                //Note, because we "send" the character right away, the transmit fifo is always empty
                char character = (char)data;
                switch (character) {
                    case '\n':
                        //End of line; print the contents of the line buffer and clear it
                        irvelog_always_stdout(
                            0,
                            "\x1b[92mRVUARTTX\x1b[0m: \"\x1b[1m%s\x1b[0m\\n\"",
                            this->m_output_line_buffer.c_str()
                        );
                        this->m_output_line_buffer.clear();
                        break;
                    case '\0':
                        //Null terminator; print the contents of the line buffer and clear it
                        //(this has helped with debugging weird issues in the past)
                        irvelog_always_stdout(
                            0,
                            "\x1b[92mRVUARTTX\x1b[0m: \"\x1b[1m%s\x1b[0m\\0\"",
                            this->m_output_line_buffer.c_str()
                        );
                        this->m_output_line_buffer.clear();
                        break;
                    case '\r':  this->m_output_line_buffer += "\x1b[0m\\r\x1b[1m"; break;//Print \r in non-bold
                    default:    this->m_output_line_buffer.push_back(character); break;
                }
            }*/
                this->m_lsr |= 0b00000001;//Set data ready bit to HIGH
                this->async_write_queue.push(data);
                this->m_lsr &= 0b11111110;//Clear data ready bit to HIGH
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

void Uart::write_thread_function(){
    //This thread will wait for data from the main thread and then print it.
    while(async_write_queue.size() == 0){
        //Wait for data to print
        while(async_write_queue.size() > 0){
            char data = char(this->async_write_queue.front());
            this->async_write_queue.pop();
            std::cout<<data<<std::flush;
        }
    }
}

void Uart::read_thread_function(){
    //This wile pole the input and push any data transmitted to the read fifo.
    //Once pushed, the main thread can pop from the fifo.
}
