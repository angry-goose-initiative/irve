/**
 * @brief   Minimal GDB server implementation to ease debugging
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

//Needed to deal with circular headers
namespace irve::internal {
    class Emulator;
}

#include "emulator.h"

/* ------------------------------------------------------------------------------------------------
 * Function Declarations
 * --------------------------------------------------------------------------------------------- */

namespace irve::internal::gdbserver {

    /**
     * @brief Start the GDB server
     * @param emulator The emulator to use
     * @param port The port to listen on
    */
    void start(
        Emulator& emulator,
        CpuState& cpu_state,
        Memory& memory,
        uint16_t port
    );
}
