/* poll_timer_interrupt_mmode.c
 * Copyright (C) 2023 John Jekel
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Repeatly poll the timer interrupt pending bit in the mip CSR
 *
*/

/* Constants And Defines */

//TODO

/* Includes */

#include <assert.h>
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

/* Types */

//TODO

/* Variables */

//TODO

/* Static Function Declarations */

//TODO

/* Function Implementations */

int main(int, const char**) {
    //TODO

    //TODO don't assume a 1000Hz timer in the future

    //size_t count = 0;
    //TODO
    //for (size_t i = 0; i < 500000

    return 0;
}

__attribute__ ((interrupt ("machine"))) void ___rvsw_exception_handler___(void) {
    assert(false && "We don't expect any exceptions to occur in this test program");
    exit(1);
}

/* Static Function Implementations */

//TODO
