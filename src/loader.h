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

/* Constants And Defines */

//TODO

/* Types */

//TODO

/* Global Variables */

//TODO

/* Function/Class Declarations */

namespace irve {
    namespace loader {
        void load_verilog_32(emulator_t& emulator, const char* filename);
    }
}

#endif//LOADER_H
