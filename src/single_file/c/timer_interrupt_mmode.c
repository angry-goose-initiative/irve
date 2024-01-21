/**
 * @file    timer_interrupt_mmode.c
 * @brief   Tests timer interrupts in machine mode
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
 * Static Variables
 * --------------------------------------------------------------------------------------------- */

static volatile bool timer_interrupt_occurred = false;

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

int main(int, const char**) {
    //Setup timer and timer compare values
    RVSW_MTIME      = 0;
    RVSW_MTIMEH     = 0;
    RVSW_MTIMECMP   = WAIT_TIME;
    RVSW_MTIMECMPH  = 0;

    RVSW_CSR_ZERO(mip);//To ensure the mtip bit starts unset
    RVSW_CSR_ZERO(mideleg);//All interrupts are handled in machine mode
    RVSW_CSRW(mie, 1 << 7);//To enable the timer interrupt (and disable all others)
    RVSW_CSRW(mstatus, 1 << 3);//To enable global machine interrupts

    printf("This program should take about %f seconds to complete\n", WAIT_TIME / 1000.0);
    while (!timer_interrupt_occurred) {
        printf("time is currently: %lu\n", RVSW_RDTIME());
    }
    printf("We broke out of the loop since a timer interrupt occurred!\n");
    printf("mip CSR is currently: %lu\n", RVSW_CSRR(mip));
    puts("Done!");

    return 0;
}

__attribute__ ((interrupt ("machine"))) void ___rvsw_exception_handler___(void) {
    assert(false && "We don't expect any exceptions to occur in this test program");
    exit(1);
}

__attribute__ ((interrupt ("machine"))) void ___rvsw_machine_timer_interrupt_handler___(void) {
    timer_interrupt_occurred = true;
    RVSW_CSR_ZERO(mie);//Disable the timer interrupt so we don't spin in this isr forever
}
