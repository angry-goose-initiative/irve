/**
 * @file    uart_read_poll_csr_interrupt_pending_test.cpp
 * @brief   Tests out the UART (reads, polling the external interrupt pending bit in mip CSR)
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
 * Static Function Declarations
 * --------------------------------------------------------------------------------------------- */

bool external_interrupt_pending(void);

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

int main(void) {
    std::cout << "This program tests reading from the UART (polling external interrupt bit pending bit in mip CSR)" << std::endl;

    std::cout << "For the next 10 seconds, characters sent to the UART will be echoed back." << std::endl;
    std::cout << "This won't just be your terminal echo, it will be very obvious when it works." << std::endl;
    std::cout << "To eliminate variables when debugging, the UART won't be used for output" << std::endl;

    RVSW_CSR_ZERO(mip);//To ensure the meip bit starts unset
    RVSW_UART_16550_IER = 0b00000001;//Enable the Recieve Data Ready interrupt

    std::cout << "Try it now!" << std::endl;

    uint32_t end_time = RVSW_MILLIS() + 10000;

    while (RVSW_MILLIS() < end_time) {
        if (external_interrupt_pending()) {
            std::cout << "Interrupt is now pending!" << std::endl;

            while (RVSW_UART_16550_LSR & (1 << 0)) {//Until the Data Ready bit is unset
                char c = RVSW_UART_16550_RHR;
                std::cout << "You just sent the character: '" << c << "'!" << std::endl;
            }

            std::cout << "Read all characters, which should have cleared pending interrupt." << std::endl;

            //The pending interrupt at the UART level will be cleared automatically when the RHR is read and FIFO emptied

            //Clear the interrupt pending bit at the CPU (HLIC) level
            RVSW_CSR_ZERO(mip);
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

/* ------------------------------------------------------------------------------------------------
 * Static Function Implementations
 * --------------------------------------------------------------------------------------------- */

bool external_interrupt_pending(void) {
    return RVSW_CSRR(mip) & (1U << 11U);
}
