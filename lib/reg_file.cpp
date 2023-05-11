/* reg_file.cpp
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 * 
 * The register file
 * Basically works like an array of 32 reg_t but index 0 will always be 0
 * 
*/

/* Includes */

#include "reg_file.h"

/* Function Implementations */

reg_file_t::reg_file_t(): regs() {}

reg_t& reg_file_t::operator[](uint8_t i) {
    assert(i < 32 && "Attempted to access invalid register");
    regs[0].u = 0;
    return regs[i];
}

const reg_t& reg_file_t::operator[](uint8_t i) const {
    assert(i < 32 && "Attempted to access invalid register");
    regs[0].u = 0;
    return regs[i];
}
