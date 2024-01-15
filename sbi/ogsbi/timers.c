/**
 * @file    timers.c
 * @brief   Timer-related code
 * 
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  See the LICENSE file at the root of the project for licensing info.
 * 
 * TODO longer description
 *
*/

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include "common.h"

#include <stdbool.h>
#include <stdint.h>

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

bool set_timer_and_clear_pending_int(uint64_t /*stime_value*/) {
    //TODO write to mtimecmp and clear the machine / (also supervisor?) timer interrupt pending bit(s)
    return false;//TODO
}
