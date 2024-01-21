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

/**
 * @brief The namespace containing IRVE's ACLINT implementation
*/
namespace irve::internal {

/* ------------------------------------------------------------------------------------------------
 * Constants/Defines
 * --------------------------------------------------------------------------------------------- */


/* ------------------------------------------------------------------------------------------------
 * Type/Class Declarations
 * --------------------------------------------------------------------------------------------- */

/**
 * @brief The IRVE ACLINT
*/
class Aclint {
public:
    enum class Address : uint16_t {
        MSWI_BEGIN   = 0x0000, // Inclusive
        MSWI_END     = 0x3FFF, // Inclusive

        MTIMER_BEGIN = 0x4000, // Inclusive
        MTIMER_END   = 0xBFFF, // Inclusive

        MTIME        = 0xBFF8,
        MTIMEH       = 0xBFFC,
        MTIMECMP     = 0x4000,
        MTIMECMPH    = 0x4004,
        //No other timecmp registers are implemented.
    };

    /**
     * @brief   The constructor
     * @param   csrs A reference to the CSR's.
    */
    Aclint(Csr& csrs);
    
    /**
     * @brief                   Read from the aclint
     * @param register_address  The register to read from
     * @return                  The register value
    */
    Word read(Aclint::Address register_address);

    /**
     * @brief                   Write to the aclint
     * @param register_address  The register to write to
     * @param data              The data to write
    */
    void write(Aclint::Address register_address, Word data);

private:
    /**
     * @brief       Reference to the CSRs since some operations depend on them.
    */
    Csr& m_csrs;
};

} // namespace irve::internal
