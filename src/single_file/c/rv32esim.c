/**
 * @file    rv32esim.c
 * @brief   Simple test code based on code from rv32esim
 * 
 * @copyright
 *  Copyright (C) 2022-2024 John Jekel\n
 *  See the LICENSE file at the root of the project for licensing info.
*/

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

int main() {
    //printf("Testing");
    puts("Hello world! C running on RISC-V!\n");
    //print_string("\xAA\xBB\xCC\xDD\xEE\xFF");
    volatile uint32_t a = 1;
    volatile uint32_t b = 2;

    assert((a + b) == 3);
    assert((a - b) == 0xFFFFFFFF);
    assert((a << b) == 4);
    assert((a >> b) == 0);
    assert(a < b);
    volatile int32_t ai = 0x80000000;
    volatile uint32_t bi = 3;
    printf("0x%lX", ai >> bi);
    putc('\n', stdout);
    assert((uint32_t)(ai >> bi) == 0xF0000000);

    assert((b * bi) == 6);
    //assert(false && "Sanity check assert works");

    return 0;
}

__attribute__ ((interrupt ("machine"))) void ___rvsw_exception_handler___(void) {
    assert(false && "We don't expect any exceptions to occur in this test program");
    exit(1);
}
