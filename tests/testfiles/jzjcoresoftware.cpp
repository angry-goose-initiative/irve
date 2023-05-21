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
#include "CSR.h"

#include <cassert>
#include <cstdint>

/* Macros */

#define setup_emulator_with_program(program_name) \
    irve::emulator::emulator_t emulator; \
    irve::loader::load_verilog_32(emulator, "jzjcoresoftware/assembly/" program_name ".hex"); \
    irve::internal::cpu_state::cpu_state_t& cpu_state_ref = emulator.m_emulator_ptr->m_cpu_state;

/* Function Implementations */

//TODO also check CSRs, memory, etc

int verify_jzjcoresoftware_adding2() {
    //Load adding2 program
    setup_emulator_with_program("adding2");

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
    emulator.run_until(0);
    
    //Checks things are as expected
    assert((emulator.get_inst_count() == 5));//3 nops, 1 auipc, 1 IRVE.EXIT
    assert((cpu_state_ref.get_pc() == 0x10));//5 4-byte instructions
    assert((cpu_state_ref.get_r(31) == ((1 << 12) + 0xC)));//Expected result of auipc

    return 0;
}

int verify_jzjcoresoftware_bneandsubtest() {
    //Load the bneandsubtest program
    setup_emulator_with_program("bneandsubtest");
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
    assert((cpu_state_ref.get_pc() == 0xc));
    assert((cpu_state_ref.get_r(31) == 1000));
    assert((cpu_state_ref.get_r(30) == 500));
    assert((cpu_state_ref.get_r(29) == 5));

    //Now we get to the loop
    uint32_t expected_x31 = 1000;
    while (expected_x31 >= 510) {
        emulator.tick();//Execute the sub
        expected_x31 -= 5;
        assert((emulator.get_inst_count() == ++expected_inst_count));
        assert((cpu_state_ref.get_pc() == 0x10));
        assert((cpu_state_ref.get_r(31) == expected_x31));
        emulator.tick();//Execute the bne
        assert((emulator.get_inst_count() == ++expected_inst_count));
        assert((cpu_state_ref.get_pc() == 0xc));
    }

    //On the last iteration, the bne should not be taken
    emulator.tick();//Execute the sub
    expected_x31 -= 5;
    assert((emulator.get_inst_count() == ++expected_inst_count));
    assert((cpu_state_ref.get_pc() == 0x10));
    assert((cpu_state_ref.get_r(31) == expected_x31));
    emulator.tick();//Execute the bne
    assert((emulator.get_inst_count() == ++expected_inst_count));
    assert((cpu_state_ref.get_pc() == 0x14));

    emulator.run_until(0);//Execute the IRVE.EXIT
    assert((emulator.get_inst_count() == ++expected_inst_count));
    assert((cpu_state_ref.get_pc() == 0x14));
    
    return 0;
}

int verify_jzjcoresoftware_callrettest() {
    //Load the callrettest program
    setup_emulator_with_program("callrettest");
    uint64_t expected_inst_count = 0;

    //Checks things are as expected
    emulator.tick();//Execute the addi
    assert((emulator.get_inst_count() == ++expected_inst_count));
    assert((cpu_state_ref.get_pc() == 0x4));
    assert((cpu_state_ref.get_r(31) == 10));
    emulator.tick();//Execute the jal (call)
    assert((emulator.get_inst_count() == ++expected_inst_count));
    assert((cpu_state_ref.get_pc() == 0xC));
    assert((cpu_state_ref.get_r(31) == 10));
    emulator.tick();//Execute the addi
    assert((emulator.get_inst_count() == ++expected_inst_count));
    assert((cpu_state_ref.get_pc() == 0x10));
    assert((cpu_state_ref.get_r(31) == 27));
    emulator.tick();//Execute the jalr (ret)
    assert((emulator.get_inst_count() == ++expected_inst_count));
    assert((cpu_state_ref.get_pc() == 0x8));
    assert((cpu_state_ref.get_r(31) == 27));
    emulator.run_until(0);//Execute the IRVE.EXIT
    assert((emulator.get_inst_count() == ++expected_inst_count));
    assert((cpu_state_ref.get_pc() == 0x8));
    assert((cpu_state_ref.get_r(31) == 27));
    
    return 0;
}

int verify_jzjcoresoftware_fenceecalltest() {
    //Load the fenceecalltest program
    setup_emulator_with_program("fenceecalltest");
    uint64_t expected_inst_count = 0;
    
    //This verifier assumes the trap vector starts at 0x4
    irve::internal::CSR::CSR_t& CSR_ref = emulator.m_emulator_ptr->m_CSR;
    assert((CSR_ref.implicit_read(irve::internal::CSR::address::MTVEC) & 0xFFFFFFFC) == (0x4 << 2));

    //Checks things are as expected
    emulator.tick();//Execute the fence
    assert((emulator.get_inst_count() == ++expected_inst_count));
    assert((cpu_state_ref.get_pc() == 0x4));
    for (uint32_t i = 0; i < 100; ++i) {//Loop a few times (this program runs forever)
        for (uint32_t j = 0; j < 7; ++j) {
            emulator.tick();//Execute an addi
            assert((emulator.get_inst_count() == ++expected_inst_count));
            assert((cpu_state_ref.get_pc() == (0x8 + (j * 4))));
        }
        emulator.tick();//Execute the ecall
        assert((emulator.get_inst_count() == expected_inst_count));//Should NOT have been retired
        assert((cpu_state_ref.get_pc() == 0x4));
    }
    
    return 0;
}

int verify_jzjcoresoftware_fibbonaccijal() {
    //Load the fibbonaccijal program
    setup_emulator_with_program("fibbonaccijal");
    uint64_t expected_inst_count = 0;
    uint32_t expected_x29 = 0;
    uint32_t expected_x30 = 1;
    uint32_t expected_x31 = 0;

    //Checks things are as expected at the start
    emulator.tick();//Execute the first addi
    assert((emulator.get_inst_count() == ++expected_inst_count));
    assert((cpu_state_ref.get_pc() == 0x4));
    assert((cpu_state_ref.get_r(29) == expected_x29));
    emulator.tick();//Execute the second addi
    assert((emulator.get_inst_count() == ++expected_inst_count));
    assert((cpu_state_ref.get_pc() == 0x8));
    assert((cpu_state_ref.get_r(30) == expected_x30));
    emulator.tick();//Execute the third addi
    assert((emulator.get_inst_count() == ++expected_inst_count));
    assert((cpu_state_ref.get_pc() == 0xC));
    assert((cpu_state_ref.get_r(31) == expected_x31));
    
    //In the loop
    for (uint32_t i = 0; i < 47; ++i) {//Max fibonacci number that can fit in a 32 bit register is the 47th
        emulator.tick();//Execute the add
        expected_x31 = expected_x29 + expected_x30;
        assert((emulator.get_inst_count() == ++expected_inst_count));
        assert((cpu_state_ref.get_pc() == 0x10));
        assert((cpu_state_ref.get_r(31) == expected_x31));
        emulator.tick();//Execute the first addi
        expected_x29 = expected_x30;
        assert((emulator.get_inst_count() == ++expected_inst_count));
        assert((cpu_state_ref.get_pc() == 0x14));
        assert((cpu_state_ref.get_r(29) == expected_x29));
        emulator.tick();//Execute the second addi
        expected_x30 = expected_x31;
        assert((emulator.get_inst_count() == ++expected_inst_count));
        assert((cpu_state_ref.get_pc() == 0x18));
        assert((cpu_state_ref.get_r(30) == expected_x30));
        emulator.tick();//Execute the jal
        assert((emulator.get_inst_count() == ++expected_inst_count));
        assert((cpu_state_ref.get_pc() == 0xC));
    }

    return 0;
}

int verify_jzjcoresoftware_fibbonaccijalr() {
    //Load the fibbonaccijal program
    setup_emulator_with_program("fibbonaccijalr");
    uint64_t expected_inst_count = 0;
    uint32_t expected_x29 = 0;
    uint32_t expected_x30 = 1;
    uint32_t expected_x31 = 0;

    //Checks things are as expected at the start
    emulator.tick();//Execute the first addi
    assert((emulator.get_inst_count() == ++expected_inst_count));
    assert((cpu_state_ref.get_pc() == 0x4));
    assert((cpu_state_ref.get_r(29) == expected_x29));
    emulator.tick();//Execute the second addi
    assert((emulator.get_inst_count() == ++expected_inst_count));
    assert((cpu_state_ref.get_pc() == 0x8));
    assert((cpu_state_ref.get_r(30) == expected_x30));
    emulator.tick();//Execute the third addi
    assert((emulator.get_inst_count() == ++expected_inst_count));
    assert((cpu_state_ref.get_pc() == 0xC));
    assert((cpu_state_ref.get_r(31) == expected_x31));
    
    //In the loop
    for (uint32_t i = 0; i < 47; ++i) {//Max fibonacci number that can fit in a 32 bit register is the 47th
        emulator.tick();//Execute the add
        expected_x31 = expected_x29 + expected_x30;
        assert((emulator.get_inst_count() == ++expected_inst_count));
        assert((cpu_state_ref.get_pc() == 0x10));
        assert((cpu_state_ref.get_r(31) == expected_x31));
        emulator.tick();//Execute the first addi
        expected_x29 = expected_x30;
        assert((emulator.get_inst_count() == ++expected_inst_count));
        assert((cpu_state_ref.get_pc() == 0x14));
        assert((cpu_state_ref.get_r(29) == expected_x29));
        emulator.tick();//Execute the second addi
        expected_x30 = expected_x31;
        assert((emulator.get_inst_count() == ++expected_inst_count));
        assert((cpu_state_ref.get_pc() == 0x18));
        assert((cpu_state_ref.get_r(30) == expected_x30));
        emulator.tick();//Execute the jalr
        assert((emulator.get_inst_count() == ++expected_inst_count));
        assert((cpu_state_ref.get_pc() == 0xC));
    }

    return 0;
}
