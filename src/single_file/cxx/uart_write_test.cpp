/**
 * @file    uart_write_test.cpp
 * @brief   Tests out the UART (writes)
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
 * Static Function Declarations
 * --------------------------------------------------------------------------------------------- */

static void double_write(const char* str);
static void uart_write_char(const char c);

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

int main(void) {
    std::cout << "This program tests writing to the UART" << std::endl;

    double_write("This text is printed twice:\n");
    double_write("- Once using your configured Newlib \"syscalls\"\n");
    double_write("- And again directly to your configured RVSW UART\n");

    std::cout << "Did it work?" << std::endl << std::endl;

    std::cout << "Okay fine, but each of those prints ended with a newline" << std::endl;
    std::cout << "Let's see if these characters show up for you right away:" << std::endl;

    uart_write_char('H');
    uart_write_char('e');
    uart_write_char('l');
    uart_write_char('l');
    uart_write_char('o');
    uart_write_char('!');

    RVSW_DELAY_MILLIS(5000);

    std::cout << "Did you see the text \"Hello!\" above BEFORE this line was printed?" << std::endl;
    std::cout << "It is not guaranteed that this would have occured, but it almost certainly should have" << std::endl;
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

static void double_write(const char* str) {
    assert(str && "SW Bug");

    std::cout << str << std::flush;

    while (*str) {
        uart_write_char(*str);
        ++str;
    }
}

static void uart_write_char(const char c) {
#if RVSW_UART_16550
    //Poll waiting for the THR to be empty, then write the character
    while ((RVSW_UART_16550_LSR & (1 << 5)) == 0) {}
    RVSW_UART_16550_THR = c;
#else
#error "RVSW configuration problem"
#endif
}
