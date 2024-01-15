/**
 * @file    cleanup.c
 * @brief   Cleans up things before rvsw C software exits.
 * 
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  See the LICENSE file at the root of the project for licensing info.
*/

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include <stdio.h>

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

void ___rvsw_cleanup___(void) {
    fflush(stdout);
    fflush(stderr);
}
