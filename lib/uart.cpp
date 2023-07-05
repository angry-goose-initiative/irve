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
        //TODO implement
        default: assert(false && "We should never get here!"); return 0;
    }
}

void uart::uart_t::write(uint8_t register_address, uint8_t data) {
    assert((register_address <= 0b111) && "Invalid UART register address!");
    switch (register_address) {
        case uart::address::THR: {//THR or DLL
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
        case uart::address::IER: {//IER or DLM
            if (this->dlab()) {//DLM
                this->m_dlm = data;
            } else {//IER
                assert(false && "TODO");//TODO
            }
            break;
        }
        case uart::address::FCR: {
            assert(false && "TODO");//TODO
            break;
        }
        case uart::address::LCR: {//LCR or PSD
            if (this->dlab()) {//PSD
                this->m_psd = data;
            } else {//LCR
                assert(false && "TODO");//TODO
            }
            break;
        }
        case uart::address::MCR: {
            assert(false && "TODO");//TODO
            break;
        }
        case uart::address::SPR: {
            this->m_spr = data;
            break;
        }
        case uart::address::LSR:
        case uart::address::MSR:
            //These registers aren't writable, but for simplicity, we'll ignore the write
            //instead of adding infastructure to report an error to the user
            break;
        default: assert(false && "We should never get here!");
    }
}

bool uart::uart_t::interrupt_pending() const {
    return this->m_isr & 0b1;
}

bool uart::uart_t::dlab() const {
    return this->m_lcr & (1 << 7);
}

/* ------------------------------------------------------------------------------------------------
 * Static Function Implementations
 * --------------------------------------------------------------------------------------------- */

//TODO
