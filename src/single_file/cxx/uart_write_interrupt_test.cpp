/**
 * @file    uart_write_interrupt_test.cpp
 * @brief   Tests out the UART (writes, external interrupts)
 * 
 * @copyright
 *  Copyright (C) 2024 John Jekel\n
 *  See the LICENSE file at the root of the project for licensing info.
 *
 *  Like uart_read_test.cpp, but polls the external interrupt bit in m
*/

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include "rvsw.h"

#include <cassert>
#include <cstdlib>
#include <iostream>

/* ------------------------------------------------------------------------------------------------
 * Static Variables
 * --------------------------------------------------------------------------------------------- */

volatile uint32_t pos;
const char* string_to_write = "This is a test string to write to the UART!\n";

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

int main(void) {
    std::cout << "This program tests writing a string to the UART using interrupts!" << std::endl;

    std::cout << "You should see the string \"" << string_to_write << "\"." << std::endl;

    RVSW_UART_16550_IER = 0b00000010;//Enable the THR empty interrupt

    RVSW_CSR_ZERO(mip);//To ensure the meip bit starts unset
    RVSW_CSR_ZERO(mideleg);//All interrupts are handled in machine mode
    RVSW_CSRW(mie, 1 << 11);//To enable the external interrupt (and disable all others)
    RVSW_CSRW(mstatus, 1 << 3);//To enable global machine interrupts

    //Write the first chracter to get the ball rolling
    pos = 1;
    RVSW_UART_16550_THR = string_to_write[0];

    //The ISR will handle the rest (updating pos until it is at the end of the string)
    while (string_to_write[pos] != '\0') {}

    std::cout << "Did it work?" << std::endl;

    return 0;
}

extern "C" __attribute__ ((interrupt ("machine"))) void ___rvsw_exception_handler___(void) {
    assert(false && "We don't expect any exceptions to occur in this test program");
    exit(1);
}

extern "C" __attribute__ ((interrupt ("machine"))) void ___rvsw_machine_external_interrupt_handler___(void) {
    assert((RVSW_UART_16550_ISR & 0x0F) == 0b0010);//Transmit Holding Register Empty was the cause of the interrupt
    assert(RVSW_UART_16550_LSR & (1 << 5));//Data Ready bit is set

    if (string_to_write[pos] != '\0') {
        RVSW_UART_16550_THR = string_to_write[pos];
        pos += 1;

        //Clear the interrupt at the CPU (HLIC) level
        RVSW_CSR_ZERO(mip);
    } else {
        //Disable the external interrupt so we don't spin in this isr forever
        RVSW_CSR_ZERO(mie);
    }
}
