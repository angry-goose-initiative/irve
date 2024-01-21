/**
 * @file    aclint.h
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

#include "CSR.h"

/**
 * @brief The namespace containing IRVE's ACLINT implementation
*/
namespace irve::internal::aclint {

/* ------------------------------------------------------------------------------------------------
 * Constants/Defines
 * --------------------------------------------------------------------------------------------- */

namespace address {
    const uint16_t MSWI_BEGIN   = 0x0000;//Inclusive
    const uint16_t MSWI_END     = 0x3FFF;//Inclusive

    const uint16_t MTIMER_BEGIN = 0x4000;//Inclusive
    const uint16_t MTIMER_END   = 0xBFFF;//Inclusive

    const uint16_t MTIME        = 0xBFF8;
    const uint16_t MTIMEH       = 0xBFFC;
    const uint16_t MTIMECMP     = 0x4000;
    const uint16_t MTIMECMPH    = 0x4004;
    //No other timecmp registers are implemented.
}

/* ------------------------------------------------------------------------------------------------
 * Type/Class Declarations
 * --------------------------------------------------------------------------------------------- */

/**
 * @brief The IRVE ACLINT
*/
class aclint_t {
public:
    /**
     * @brief   The constructor
     * @param   csrs A reference to the CSR's.
    */
    aclint_t(CSR::CSR_t& csrs);
    
    /**
     * @brief                   Read from the aclint
     * @param register_address  The register to read from
     * @return                  The register value
    */
    word_t read(uint16_t register_address);

    /**
     * @brief                   Write to the aclint
     * @param register_address  The register to write to
     * @param data              The data to write
    */
    void write(uint16_t register_address, word_t data);

private:
    /**
     * @brief       Reference to the CSRs since some operations depend on them.
    */
    CSR::CSR_t& m_csrs;
};

}//namespace irve::internal::aclint
