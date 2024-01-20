/**
 * @file    poll_timer_interrupt_mmode.c
 * @brief   Repeatly poll the timer interrupt pending bit in the mip CSR
 * 
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  See the LICENSE file at the root of the project for licensing info.
*/

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include "rvsw.h"

#include <assert.h>
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

/* ------------------------------------------------------------------------------------------------
 * Constants/Defines
 * --------------------------------------------------------------------------------------------- */

#define WAIT_TIME 2000

/* ------------------------------------------------------------------------------------------------
 * Static Function Declarations
 * --------------------------------------------------------------------------------------------- */

bool timer_interrupt_pending(void);

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

int main(int, const char**) {
    RVSW_MTIME      = 0;
    RVSW_MTIMEH     = 0;
    RVSW_MTIMECMP   = WAIT_TIME;
    RVSW_MTIMECMPH  = 0;

    RVSW_CSR_ZERO(mip);//To ensure the mtip bit starts unset

    printf("This program should take about %f seconds to complete\n", WAIT_TIME / 1000.0);
    while (!timer_interrupt_pending()) {
        printf("time is currently: %lu\n", RVSW_RDTIME());
    }
    printf("mip CSR is currently: %lu\n", RVSW_CSRR(mip));
    puts("Done!");

    return 0;
}

__attribute__ ((interrupt ("machine"))) void ___rvsw_exception_handler___(void) {
    assert(false && "We don't expect any exceptions to occur in this test program");
    exit(1);
}

/* ------------------------------------------------------------------------------------------------
 * Static Function Implementations
 * --------------------------------------------------------------------------------------------- */

bool timer_interrupt_pending(void) {
    return RVSW_CSRR(mip) & (1 << 7);
}
