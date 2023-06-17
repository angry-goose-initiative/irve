/* cleanup.c
 * Copyright (C) 2023 John Jekel
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Cleans up things before rvsw C software exits.
 *
*/

/* Includes */

#include <stdio.h>

/* Function Implementations */

void ___rvsw_cleanup___(void) {
    fflush(stdout);
    fflush(stderr);

    //TODO do Newlib atexit() things
}
