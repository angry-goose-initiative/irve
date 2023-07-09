/**
 * @file    semihosting.cpp
 * @brief   M-Mode semihosting support for IRVE
 * 
 * @copyright Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 * 
 * TODO longer description
 *
*/

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include "semihosting.h"

#include "cpu_state.h"
#include "memory.h"

#define INST_COUNT 0
#include "logging.h"

using namespace irve::internal;

/* ------------------------------------------------------------------------------------------------
 * Constants/Defines
 * --------------------------------------------------------------------------------------------- */

#define a0 10
#define a1 11

/* ------------------------------------------------------------------------------------------------
 * Type/Class Declarations
 * --------------------------------------------------------------------------------------------- */

//TODO

/* ------------------------------------------------------------------------------------------------
 * Static Variables
 * --------------------------------------------------------------------------------------------- */

//TODO

/* ------------------------------------------------------------------------------------------------
 * Static Function Declarations
 * --------------------------------------------------------------------------------------------- */

//TODO

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

void semihosting::handle(cpu_state::cpu_state_t& cpu_state, memory::memory_t& /*memory*//*, const CSR::CSR_t& CSR*/) {
    switch (cpu_state.get_r(a0).u) {
        case 0x03://SYS_WRITEC
            //TODO actually save this into some buffer and print out a whole line at a time
            irvelog_always_stdout(0, "SYS_WRITEC: %c", cpu_state.get_r(a1));
            break;
        default:
            irvelog_always_stdout(0, "Semihosting call 0x%08x not implemented\n", cpu_state.get_r(a0));//TODO should this be always?
            break;
    }
    cpu_state.goto_next_sequential_pc();
}

/* ------------------------------------------------------------------------------------------------
 * Static Function Implementations
 * --------------------------------------------------------------------------------------------- */

//TODO
