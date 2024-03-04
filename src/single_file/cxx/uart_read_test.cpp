/**
 * @file    uart_read_test.cpp
 * @brief   Tests out the UART (reads)
 * 
 * @copyright
 *  Copyright (C) 2024 John Jekel\n
 *  See the LICENSE file at the root of the project for licensing info.
*/

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include "rvsw.h"

#include <cassert>
#include <cstdlib>
#include <iostream>

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

int main(void) {
    std::cout << "This program tests reading from the UART (polling the LSR's Data Ready bit)" << std::endl;

    std::cout << "For the next 10 seconds, characters sent to the UART will be echoed back." << std::endl;
    std::cout << "This won't just be your terminal echo, it will be very obvious when it works." << std::endl;
    std::cout << "To eliminate variables when debugging, the UART won't be used for output" << std::endl;

    std::cout << "Try it now!" << std::endl;

    uint32_t end_time = RVSW_MILLIS() + 10000;

    while (RVSW_MILLIS() < end_time) {
        if (RVSW_UART_16550_LSR & (1 << 0)) {//Poll Data Ready bit
            char c = RVSW_UART_16550_RHR;
            std::cout << "You just sent the character: '" << c << "'!" << std::endl;
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
