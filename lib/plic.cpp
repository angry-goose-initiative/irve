/**
 * @file    plic.cpp
 * @brief   TODO
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

#include "plic.h"

using namespace irve::internal;

/* ------------------------------------------------------------------------------------------------
 * Constants/Defines
 * --------------------------------------------------------------------------------------------- */

//TODO

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

plic::plic_t::plic_t() {
    //TODO
}

plic::plic_t::~plic_t() {
    //TODO
}

word_t plic::plic_t::read(word_t register_address) {
    assert((register_address.u < 0x04000000) && "Attempt to access out of bounds PLIC register");
    
    return 0; assert(false && "TODO");
}

void plic::plic_t::write(word_t register_address, word_t /*data*/) {
    assert((register_address.u < 0x04000000) && "Attempt to access out of bounds PLIC register");

    assert(false && "TODO");
}

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

//TODO

/* ------------------------------------------------------------------------------------------------
 * Static Function Implementations
 * --------------------------------------------------------------------------------------------- */

//TODO
