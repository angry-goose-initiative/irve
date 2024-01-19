/**
 * @file    semihosting.h
 * @brief   M-Mode semihosting support for IRVE
 * 
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  See the LICENSE file at the root of the project for licensing info.
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

namespace irve::internal::semihosting {
    /**
     * @brief   Semihosting handler
     * 
     * TODO
     * 
     */
    class handler_t {
    public:
        handler_t() = default;
        ~handler_t();
        void handle(cpu_state::cpu_state_t& cpu_state, memory::memory_t& memory/*, const CSR::CSR_t& CSR*/);
    private:
        std::string m_output_line_buffer;
    };
}

//TODO

/* ------------------------------------------------------------------------------------------------
 * Function Declarations
 * --------------------------------------------------------------------------------------------- */

#endif//SEMIHOSTING_H
