/**
 * @file    init.cpp
 * @brief   Initialization tests
 * 
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  See the LICENSE file at the root of the project for licensing info.
 *
 * Performs integration tests to ensure that the emulator's initial state
 * is correct and that loading works
*/

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include "irve_public_api.h"

#undef NDEBUG//Asserts should work even in release mode for tests
#include <cassert>

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

int test_emulator_t_sanity() {//We can create an emulator, run it for 1000 cycles, and it doesn't crash
    irve::emulator::emulator_t emulator(0, nullptr);
    emulator.run_until(1000);
    return 0;
}

int test_emulator_t_init() {//The emulator's initial state is correct
    irve::emulator::emulator_t emulator(0, nullptr);

    //TODO
    return 0;
}
