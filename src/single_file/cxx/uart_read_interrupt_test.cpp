/**
 * @file    uart_read_interrupt_test.cpp
 * @brief   Tests out the UART (reads, external interrupts)
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

volatile bool interrupt_occurred = false;
volatile char character_received;

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

int main(void) {
    std::cout << "This program tests reading from the UART (full external interrupt test)" << std::endl;

    std::cout << "For the next 10 seconds, characters sent to the UART will be echoed back." << std::endl;
    std::cout << "This won't just be your terminal echo, it will be very obvious when it works." << std::endl;
    std::cout << "To eliminate variables when debugging, the UART won't be used for output" << std::endl;

    RVSW_UART_16550_IER = 0b00000001;//Enable the Recieve Data Ready interrupt

    RVSW_CSR_ZERO(mip);//To ensure the meip bit starts unset
    RVSW_CSR_ZERO(mideleg);//All interrupts are handled in machine mode
    RVSW_CSRW(mie, 1 << 11);//To enable the external interrupt (and disable all others)
    RVSW_CSRW(mstatus, 1 << 3);//To enable global machine interrupts

    std::cout << "Try it now!" << std::endl;

    uint32_t end_time = RVSW_MILLIS() + 10000;

    while (RVSW_MILLIS() < end_time) {
        if (interrupt_occurred) {
            interrupt_occurred = false;
            std::cout << "Interrupt occured!" << std::endl;
            std::cout << "You just sent the character: '" << character_received << "'!" << std::endl;

            //Clear the interrupt pending bit at the CPU (HLIC) level
            RVSW_CSR_ZERO(mip);

            //The pending interrupt at the UART level will be cleared automatically when the RHR is read and FIFO emptied
            //If there are still characters in the FIFO however, re-enabling int interrupts here may cause another to
            //fire immediately. But this is okay because we'll catch it in the next iteration of the loop!
            RVSW_CSRW(mie, 1 << 11);
        }
    }

    std::cout << "Ten seconds is up, did it work for you?" << std::endl;
    std::cout << "Best of luck!" << std::endl;

    return 0;
}

extern "C" __attribute__ ((interrupt ("machine"))) void ___rvsw_exception_handler___(void) {
    assert(false && "We don't expect any exceptions to occur in this test program");
    exit(1);
}

extern "C" __attribute__ ((interrupt ("machine"))) void ___rvsw_machine_external_interrupt_handler___(void) {
    assert((RVSW_UART_16550_ISR & 0x0F) == 0b0100);//Recieve Data Ready was the cause of the interrupt
    assert(RVSW_UART_16550_LSR & 0b1);//Data Ready bit is set

    interrupt_occurred = true;
    character_received = RVSW_UART_16550_RHR;

    //Clear the interrupt at the CPU (HLIC) level
    RVSW_CSR_ZERO(mip);

    //Disable the external interrupt so we don't spin in this isr forever, or lose characters
    //if there are multiple in the FIFO for example.
    RVSW_CSR_ZERO(mie);
}
