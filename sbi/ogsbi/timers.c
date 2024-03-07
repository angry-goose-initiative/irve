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

#include "rvsw.h"

#include "common.h"

#include <stdbool.h>
#include <stdint.h>

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

bool set_timer_and_clear_pending_int(uint64_t stime_value) {
    //Set the timer as requested (in the way suggested by the RISC-V spec, vol 2 page 46)
    RVSW_MTIMECMP   = 0xFFFFFFFF;
    RVSW_MTIMECMPH  = (uint32_t)((stime_value >> 32)    & 0xFFFFFFFF);
    RVSW_MTIMECMP   = (uint32_t)(stime_value            & 0xFFFFFFFF);

    //Clear the pending timer interrupt (m-mode and s-mode respectively)
    RVSW_CSRW(mip, RVSW_CSRR(mip) & ~(1U << 7U));
    RVSW_CSRW(mip, RVSW_CSRR(mip) & ~(1U << 5U));

    //Enable machine-mode timer interrupts so we can forward the next one to supervisor mode
    //The forwarder in `interrupt_entry.S` will then disable the machine-mode timer interrupt
    //In this was we won't loop in m-mode forever!
    RVSW_CSRW(mie, RVSW_CSRR(mie) | (1U << 7U));

    return true;//We were successful!
}
