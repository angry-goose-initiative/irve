/**
 * @brief   Implementation of RISC-V ACLINT
 * 
 * @copyright
 *  Copyright (C) 2024 John Jekel\n
 *  See the LICENSE file at the root of the project for licensing info.
 *
*/

#pragma once

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include <stdint.h>

#include "common.h"
#include "csr.h"

/* ------------------------------------------------------------------------------------------------
 * Type/Class Declarations
 * --------------------------------------------------------------------------------------------- */

namespace irve::internal {

class Aclint {
public:
    // Aclint register addresses
    enum class Address : uint16_t {
        MSWI_BEGIN   = 0x0000, // Inclusive
        MSWI_END     = 0x3FFF, // Inclusive

        MTIMER_BEGIN = 0x4000, // Inclusive
        MTIMER_END   = 0xBFFF, // Inclusive

        MTIME        = 0xBFF8,
        MTIMEH       = 0xBFFC,
        MTIMECMP     = 0x4000,
        MTIMECMPH    = 0x4004
        // No other timecmp registers are implemented.
    };

    Aclint(Csr& csr);

    Aclint() = delete;
    
    Word read(Aclint::Address register_address);

    void write(Aclint::Address register_address, Word data);

private:

    Csr& m_csrs; // Reference to the CSRs since some operations depend on them.

};

} // namespace irve::internal
