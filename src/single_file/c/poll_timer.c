/**
 * @file    poll_timer.c
 * @brief   Repeatly poll the RISC-V timer (m-mode or s-mode)
 * 
 * @copyright
 *  Copyright (C) 2024 John Jekel\n
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
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

int main(int, const char**) {
    printf("This program should take about %f seconds to complete\n", (WAIT_TIME / 1000.0) * 2.0);

    printf("Polling CSR until %u ms have elapsed...\n", WAIT_TIME);
    //RVSW_CSRW(time, 0);//The time CSR is only a read-only shadow of the memory-mapped mtime register, so this would cause an illegal instruction exception
    RVSW_MTIME = 0;
    while (RVSW_RDTIME() < WAIT_TIME) {
        printf("time is currently: %lu\n", RVSW_RDTIME());
    }
    puts("Done!");

    printf("Polling memory address until %u ms have elapsed...\n", WAIT_TIME);
    RVSW_MTIME = 0;
    while (RVSW_MTIME < WAIT_TIME) {
        printf("mtime is currently: %lu\n", RVSW_MTIME);
    }
    puts("Done!");

    return 0;
}

//Regardless of the mode, this will exit, so we don't need to worry about "machine" or "supervisor" here
__attribute__ ((interrupt ("machine"))) void ___rvsw_exception_handler___(void) {
    assert(false && "We don't expect any exceptions to occur in this test program");
    exit(1);
}
