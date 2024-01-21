/**
 * @brief   TODO
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

#include <cstdint>

#include "common.h"

/* ------------------------------------------------------------------------------------------------
 * Constants/Defines
 * --------------------------------------------------------------------------------------------- */

//TODO

/* ------------------------------------------------------------------------------------------------
 * Type/Class Declarations
 * --------------------------------------------------------------------------------------------- */

/**
 * @brief The namespace containing the IRVE's PLIC implementation
*/
namespace irve::internal::plic {
    //TODO
    /**
     * @brief The IRVE RISC-V PLIC
    */
    class plic_t {
    public:
        plic_t();
        ~plic_t();

        //26-bit addresses, 32-bit data
        Word read(Word register_address);//NOT const since it could pop from the FIFO
        void write(Word register_address, Word data);

        //TODO other functions to check for MEI and SEI (if they should be set)
    };
}

/* ------------------------------------------------------------------------------------------------
 * Function Declarations
 * --------------------------------------------------------------------------------------------- */

//TODO
