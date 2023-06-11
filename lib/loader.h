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

/**
 * @brief Contains functions to load binaries into the emulator's memory from a file
*/
namespace irve::internal::loader {
    /**
     * @brief Loads a RISC-V binary into the emulator's memory (in the Verilog format, 32-bits wide, big-endian)
     * @param emulator A reference to the emulator to load the binary into
     * @param filename The name of the file to load
     * @return True if the binary was loaded successfully, false otherwise
    */
    void load_verilog_32(irve::internal::emulator::emulator_t& emulator, const char* filename);
}

#endif//LOADER_H
