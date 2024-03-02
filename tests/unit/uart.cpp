/**
 * @file    uart.cpp
 * @brief   Tests for the IRVE UART
 * 
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  See the LICENSE file at the root of the project for licensing info.
 * 
 * TODO longer description
 *
*/

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#undef NDEBUG//Asserts should work even in release mode for tests
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

/* ------------------------------------------------------------------------------------------------
 * Static Function Declarations
 * --------------------------------------------------------------------------------------------- */

//TODO

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

int test_uart_Uart_sanity() {
    Uart uart;

    uart.write(Uart::Address::THR, 'I');
    uart.write(Uart::Address::THR, 'R');
    uart.write(Uart::Address::THR, 'V');
    uart.write(Uart::Address::THR, 'E');
    uart.write(Uart::Address::THR, '\n');

    return 0;
}

int test_uart_Uart_init() {
    Uart uart;

    assert(uart.read(Uart::Address::IER) == 0x00);
    assert(uart.read(Uart::Address::ISR) == 0x01);
    //assert(uart.read(Uart::Address::FCR) == 0x00);//Sadly this register isn't readable
    assert(uart.read(Uart::Address::LCR) == 0x03);//Instead of 0x00 since we only support 8-bit characters
    assert(uart.read(Uart::Address::MCR) == 0x00);
    assert(uart.read(Uart::Address::LSR) == 0x60);
    //Not testing MSR since there is some reset weirdness
    assert(uart.read(Uart::Address::SPR) == 0x00);

    uart.write(Uart::Address::LCR, (1 << 7) | 0x03);//Set LCR.DLAB
    //No need to test DLL or DLM since they are undefined at reset
    assert(uart.read(Uart::Address::PSD) == 0x00);

    return 0;
}

/* ------------------------------------------------------------------------------------------------
 * Static Function Implementations
 * --------------------------------------------------------------------------------------------- */

//TODO
