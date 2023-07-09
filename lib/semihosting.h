/**
 * @file    semihosting.h
 * @brief   M-Mode semihosting support for IRVE
 * 
 * @copyright Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 * 
 * TODO longer description
 *
*/

#ifndef SEMIHOSTING_H
#define SEMIHOSTING_H

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include "cpu_state.h"
#include "memory.h"

/* ------------------------------------------------------------------------------------------------
 * Constants/Defines
 * --------------------------------------------------------------------------------------------- */

//TODO

/* ------------------------------------------------------------------------------------------------
 * Type/Class Declarations
 * --------------------------------------------------------------------------------------------- */

//TODO

/* ------------------------------------------------------------------------------------------------
 * Function Declarations
 * --------------------------------------------------------------------------------------------- */

namespace irve::internal::semihosting {
    void handle(cpu_state::cpu_state_t& cpu_state, memory::memory_t& memory/*, const CSR::CSR_t& CSR*/);
}

#endif//SEMIHOSTING_H
