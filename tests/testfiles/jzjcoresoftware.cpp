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
#include <cstdint>

/* Macros */

#define setup_emulator_with_program(program_name) \
    irve::emulator::emulator_t emulator; \
    irve::loader::load_verilog_32(emulator, "jzjcoresoftware/assembly/" program_name ".hex"); \

/* Function Implementations */

//TODO also check CSRs, memory, etc

int verify_jzjcoresoftware_adding2() {
    //Load adding2 program
    setup_emulator_with_program("adding2");
    irve::internal::cpu_state::cpu_state_t& cpu_state_ref = emulator.m_emulator_ptr->m_cpu_state;

    //Ensure all the adds are working
    for (uint32_t i = 0; i < 4096; ++i) {
        assert((emulator.get_inst_count() == i));//1 instruction per loop
        assert((cpu_state_ref.get_pc() == (i * 4)));//4 bytes per instruction
        assert((cpu_state_ref.get_r(31) == (i * 2)));//Expected result of adding2 each time
        emulator.tick();
    }

    //Ensure the program exits at instruction 4097
    emulator.run_until(0);
    assert((emulator.get_inst_count() == 4097));
    assert((cpu_state_ref.get_pc() == (4096 * 4)));//Since 1st instruction is at 0
    assert((cpu_state_ref.get_r(31) == 8192));//Since last instruction is IRVE.EXIT

    return 0;
}

int verify_jzjcoresoftware_auipctest() {
    //Load the auipctest program
    setup_emulator_with_program("auipctest");
    irve::internal::cpu_state::cpu_state_t& cpu_state_ref = emulator.m_emulator_ptr->m_cpu_state;
    emulator.run_until(0);
    
    //Checks things are as expected
    assert((emulator.get_inst_count() == 5));//3 nops, 1 auipc, 1 IRVE.EXIT
    assert((cpu_state_ref.get_pc() == 0x10));//5 4-byte instructions
    assert((cpu_state_ref.get_r(31) == ((1 << 12) + 0xC)));//Expected result of auipc

    return 0;
}

int verify_jzjcoresoftware_bneandsubtest() {
    //Load the auipctest program
    setup_emulator_with_program("bneandsubtest");
    irve::internal::cpu_state::cpu_state_t& cpu_state_ref = emulator.m_emulator_ptr->m_cpu_state;
    uint64_t expected_inst_count = 0;
    
    //Checks things are as expected
    emulator.tick();
    assert((emulator.get_inst_count() == ++expected_inst_count));
    assert((cpu_state_ref.get_pc() == 0x4));
    assert((cpu_state_ref.get_r(31) == 1000));
    emulator.tick();
    assert((emulator.get_inst_count() == ++expected_inst_count));
    assert((cpu_state_ref.get_pc() == 0x8));
    assert((cpu_state_ref.get_r(31) == 1000));
    assert((cpu_state_ref.get_r(30) == 500));
    emulator.tick();
    assert((emulator.get_inst_count() == ++expected_inst_count));
    assert((cpu_state_ref.get_pc() == 0xC));
    assert((cpu_state_ref.get_r(31) == 1000));
    assert((cpu_state_ref.get_r(30) == 500));
    assert((cpu_state_ref.get_r(29) == 5));

    //Now we get to the loop
    uint32_t expected_r31 = 1000;
    while (expected_r31 >= 510) {
        emulator.tick();//Execute the sub
        expected_r31 -= 5;
        assert((emulator.get_inst_count() == ++expected_inst_count));
        assert((cpu_state_ref.get_pc() == 0x10));
        assert((cpu_state_ref.get_r(31) == expected_r31));
        emulator.tick();//Execute the bne
        assert((emulator.get_inst_count() == ++expected_inst_count));
        assert((cpu_state_ref.get_pc() == 0xC));
    }

    //On the last iteration, the bne should not be taken
    emulator.tick();//Execute the sub
    expected_r31 -= 5;
    assert((emulator.get_inst_count() == ++expected_inst_count));
    assert((cpu_state_ref.get_pc() == 0x10));
    assert((cpu_state_ref.get_r(31) == expected_r31));
    emulator.tick();//Execute the bne
    assert((emulator.get_inst_count() == ++expected_inst_count));
    assert((cpu_state_ref.get_pc() == 0x14));

    emulator.run_until(0);//Execute the IRVE.EXIT
    assert((emulator.get_inst_count() == ++expected_inst_count));
    assert((cpu_state_ref.get_pc() == 0x14));
    
    return 0;
}