/**
 * @brief   M-Mode semihosting support for IRVE
 * 
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  See the LICENSE file at the root of the project for licensing info.
 * 
 * TODO longer description
 *
*/

#pragma once

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include "cpu_state.h"
#include "memory.h"

/* ------------------------------------------------------------------------------------------------
 * Type/Class Declarations
 * --------------------------------------------------------------------------------------------- */

namespace irve::internal {

class SemihostingHandler {
public:
    SemihostingHandler() = default;
    ~SemihostingHandler();
    void handle(CpuState& cpu_state, Memory& memory/*, const Csr& CSR*/);
private:
    std::string m_output_line_buffer;
};

} // namespace irve::internal 
