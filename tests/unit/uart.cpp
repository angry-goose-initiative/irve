/**
 * @file    uart.cpp
 * @brief   Tests for the IRVE UART
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

#include "uart.h"

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
//TODO

/* ------------------------------------------------------------------------------------------------
 * Static Function Declarations
 * --------------------------------------------------------------------------------------------- */

//TODO

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

int test_uart_uart_t_sanity() {
    uart::uart_t uart;

    //FIXME init the uart first

    uart.write(uart::address::THR, 'I');
    uart.write(uart::address::THR, 'R');
    uart.write(uart::address::THR, 'V');
    uart.write(uart::address::THR, 'E');
    uart.write(uart::address::THR, '\n');

    return 0;
}

int test_uart_uart_t_init() {
    uart::uart_t uart;

    assert(uart.read(uart::address::RHR) == 0x00);
    assert(uart.read(uart::address::THR) == 0x00);
    assert(uart.read(uart::address::IER) == 0x00);
    assert(uart.read(uart::address::ISR) == 0x01);
    assert(uart.read(uart::address::FCR) == 0x00);
    assert(uart.read(uart::address::LCR) == 0x00);
    assert(uart.read(uart::address::MCR) == 0x00);
    assert(uart.read(uart::address::LSR) == 0x60);
    //Not testing MSR since there is some reset weirdness
    assert(uart.read(uart::address::SPR) == 0x00);

    //No need to test DLL or DLM since they are undefined at reset
    uart.write(uart::address::LCR, (1 << 7));//Set LCR.DLAB
    assert(uart.read(uart::address::PSD) == 0x00);

    return 0;
}

/* ------------------------------------------------------------------------------------------------
 * Static Function Implementations
 * --------------------------------------------------------------------------------------------- */

//TODO
