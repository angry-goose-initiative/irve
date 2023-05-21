/* jzjcoresoftware.cpp
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Performs integration tests to ensure that assembly programs originally
 * from JZJCoreSoftware run as expected
 *
*/

/* Includes */

#define private public//Since we need to access internal emulator state for testing

#include "irve_public_api.h"
#include "emulator.h"
#include "cpu_state.h"

#include <cassert>

/* Function Implementations */

int test_jzjcoresoftware_auipctest() {
    //Load and run the auipctest program
    irve::emulator::emulator_t emulator;
    irve::loader::load_verilog_32(emulator, "jzjcoresoftware/assembly/auipctest.hex");
    emulator.run_until(0);
    
    //Checks things are as expected
    assert((emulator.get_inst_count() == 5));//3 nops, 1 auipc, 1 IRVE.EXIT
    irve::internal::cpu_state::cpu_state_t& cpu_state_ref = emulator.m_emulator_ptr->m_cpu_state;
    assert((cpu_state_ref.get_pc() == 0x10));//5 4-byte instructions
    assert((cpu_state_ref.get_r(31) == ((1 << 12) + 0xC)));//Expected result of auipc

    return 0;
}
