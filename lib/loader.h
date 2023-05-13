/* loader.cpp
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Code for loading RISC-V binaries into memory
 *
*/

#ifndef LOADER_H
#define LOADER_H

/* Includes */

#include "emulator.h"

//TODO namespacing

/* Function/Class Declarations */

namespace irve::internal::loader {
    void load_verilog_32(irve::internal::emulator::emulator_t& emulator, const char* filename);
}

#endif//LOADER_H
