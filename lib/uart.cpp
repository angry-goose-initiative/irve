/**
 * @file    uart.cpp
 * @brief   16550 UART implementation
 * 
 * @copyright Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
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

#include "uart.h"

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

uart::uart_t::uart_t() {
    //TODO
}

uart::uart_t::~uart_t() {
    if (this->m_output_line_buffer.size() > 0) {
        irvelog_always_stdout(
            0,
            "\x1b[92mRV:\x1b[0m: \"\x1b[1m%s\x1b[0m\"",
            this->m_output_line_buffer.c_str()
        );
    }
}

uint8_t uart::uart_t::read(uint8_t register_address) {
    assert((register_address <= 0b111) && "Invalid UART register address!");
    switch (register_address) {
        case address::RHR: {//RHR or DLL (Never THR since that isn't readable)
            if (this->dlab()) {//DLL
                return this->m_dll;
            } else {//RHR
                return std::getchar();//TODO is the fact that this is blocking a problem?
            }
            break;
        }
        case address::IER: {//IER or DLM
            if (this->dlab()) {//DLM
                return this->m_dlm;
            } else {//IER
                assert(false && "TODO");//TODO
            }
            break;
        }
        case address::ISR: {//NOTE: Never FCR since that isn't readable
            assert(false && "TODO");//TODO
            break;
        }
        case address::LCR: {
            assert(false && "TODO");//TODO
            break;
        }
        case address::MCR: {
            assert(false && "TODO");//TODO
            break;
        }
        case address::LSR: {//NOTE: Never PSD since that isn't readable
            assert(false && "TODO");//TODO
            break;
        }
        case address::MSR: {
            assert(false && "TODO");//TODO
            break;
        }
        case address::SPR: {
            return this->m_spr;
            break;
        }

        default: assert(false && "We should never get here!"); return 0;
    }
}

void uart::uart_t::write(uint8_t register_address, uint8_t data) {
    assert((register_address <= 0b111) && "Invalid UART register address!");
    switch (register_address) {
        case address::THR: {//THR or DLL (Never RHR since that isn't writable)
            if (this->dlab()) {//DLL
                this->m_dll = data;
            } else {//THR
                //Note, because we "send" the character right away, the transmit fifo is always empty
                if (((char)data) == '\n') {
                    //End of line; print the contents of the line buffer to stdout
                    irvelog_always_stdout(
                        0,
                        "\x1b[92mRV\x1b[0m: \"\x1b[1m%s\x1b[0m\\n\"",
                        this->m_output_line_buffer.c_str()
                    );
                    this->m_output_line_buffer.clear();
                } else {
                    this->m_output_line_buffer.push_back((char)data);
                }
            }
            break;
        }
        case address::IER: {//IER or DLM
            if (this->dlab()) {//DLM
                this->m_dlm = data;
            } else {//IER
                assert(false && "TODO");//TODO
            }
            break;
        }
        case address::FCR: {//NOTE: Never ISR since that isn't writable
            assert(false && "TODO");//TODO
            break;
        }
        case address::LCR: {
            assert(false && "TODO");//TODO
            break;
        }
        case address::MCR: {
            assert(false && "TODO");//TODO
            break;
        }
        case address::PSD://NOTE: Never LSR since that isn't writable
            assert(this->dlab() && "TODO software was mean");//TODO
            this->m_psd = data;
            break;
        case address::MSR://NOTE: Should never be written to by software
            assert(false && "TODO software was mean");//TODO
            break;
        case uart::address::SPR: {
            this->m_spr = data;
            break;
        }
        default: assert(false && "We should never get here!");
    }
}

bool uart::uart_t::interrupt_pending() const {
    return this->m_isr & 0b1;
}

bool uart::uart_t::dlab() const {
    return this->m_lcr & (1 << 7);
}
