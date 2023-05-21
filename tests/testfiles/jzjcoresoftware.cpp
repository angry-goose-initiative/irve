/* jzjcoresoftware.cpp
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Verifies that assembly programs originally from JZJCoreSoftware run as expected
 *
*/

/* Includes */

#define private public//Since we need to access internal emulator state for testing

#include "irve_public_api.h"
#include "emulator.h"
#include "cpu_state.h"
#include "CSR.h"
#include "memory.h"

#include <cassert>
#include <cstdint>

/* Macros */

#define setup_emulator_with_program(program_name) \
    irve::emulator::emulator_t emulator; \
    irve::loader::load_verilog_32(emulator, "jzjcoresoftware/assembly/" program_name ".hex"); \
    irve::internal::cpu_state::cpu_state_t& cpu_state_ref = emulator.m_emulator_ptr->m_cpu_state;

/* Static Function Declarations */

static int verify_jzjcoresoftware_fibonacci(irve::emulator::emulator_t& emulator, irve::internal::cpu_state::cpu_state_t& cpu_state_ref);

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

    return verify_jzjcoresoftware_fibonacci(emulator, cpu_state_ref);
}

int verify_jzjcoresoftware_fibbonaccijalr() {
    //Load the fibbonaccijal program
    setup_emulator_with_program("fibbonaccijalr");

    return verify_jzjcoresoftware_fibonacci(emulator, cpu_state_ref);
}

int verify_jzjcoresoftware_lbutest() {
    //Load the lbutest program
    setup_emulator_with_program("lbutest");
    uint64_t expected_inst_count = 0;

    //This testcase assumes x30 starts at 0 (it forgot to set it)
    cpu_state_ref.set_r(30, 0);

    //Check the loop
    const uint32_t expected_x31[16] = {
        0x01, 0x23, 0x45, 0x67,
        0x89, 0xAB, 0xCD, 0xEF,
        0xFE, 0xDC, 0xBA, 0x98,
        0x76, 0x54, 0x32, 0x10
    };
    for (uint32_t i = 0; i < 16; ++i) {
        emulator.tick();//Execute the lbu
        assert((emulator.get_inst_count() == ++expected_inst_count));
        assert((cpu_state_ref.get_pc() == 0x4));
        assert((cpu_state_ref.get_r(31) == expected_x31[i]));
        emulator.tick();//Execute the addi
        assert((emulator.get_inst_count() == ++expected_inst_count));
        assert((cpu_state_ref.get_pc() == 0x8));
        assert((cpu_state_ref.get_r(30) == (i + 1)));
        emulator.tick();//Execute the jalr
        assert((emulator.get_inst_count() == ++expected_inst_count));
        assert((cpu_state_ref.get_pc() == 0x0));
    }

    return 0;
}

int verify_jzjcoresoftware_lhtest() {
    //Load the lhtest program
    setup_emulator_with_program("lhtest");
    uint64_t expected_inst_count = 0;

    //This testcase assumes x30 starts at 0 (it forgot to set it)
    cpu_state_ref.set_r(30, 0);

    //Check the loop
    const uint32_t expected_x31[8] = {
        0x2301, 0x6745,
        0xFFFFAB89, 0xFFFFEFCD,//Sign extension
        0xFFFFDCFE, 0xFFFF98BA,//Sign extension
        0x5476, 0x1032
    };
    for (uint32_t i = 0; i < 8; ++i) {
        emulator.tick();//Execute the lh
        assert((emulator.get_inst_count() == ++expected_inst_count));
        assert((cpu_state_ref.get_pc() == 0x4));
        assert((cpu_state_ref.get_r(31) == expected_x31[i]));
        emulator.tick();//Execute the addi
        assert((emulator.get_inst_count() == ++expected_inst_count));
        assert((cpu_state_ref.get_pc() == 0x8));
        assert((cpu_state_ref.get_r(30) == ((i + 1) * 2)));
        emulator.tick();//Execute the jalr
        assert((emulator.get_inst_count() == ++expected_inst_count));
        assert((cpu_state_ref.get_pc() == 0x0));
    }

    return 0;
}

int verify_jzjcoresoftware_lhutest() {
    //Load the lhutest program
    setup_emulator_with_program("lhutest");
    uint64_t expected_inst_count = 0;

    //This testcase assumes x30 starts at 0 (it forgot to set it)
    cpu_state_ref.set_r(30, 0);

    //Check the loop
    const uint32_t expected_x31[8] = {
        0x2301, 0x6745,
        0xAB89, 0xEFCD,
        0xDCFE, 0x98BA,
        0x5476, 0x1032
    };
    for (uint32_t i = 0; i < 8; ++i) {
        emulator.tick();//Execute the lhu
        assert((emulator.get_inst_count() == ++expected_inst_count));
        assert((cpu_state_ref.get_pc() == 0x4));
        assert((cpu_state_ref.get_r(31) == expected_x31[i]));
        emulator.tick();//Execute the addi
        assert((emulator.get_inst_count() == ++expected_inst_count));
        assert((cpu_state_ref.get_pc() == 0x8));
        assert((cpu_state_ref.get_r(30) == ((i + 1) * 2)));
        emulator.tick();//Execute the jalr
        assert((emulator.get_inst_count() == ++expected_inst_count));
        assert((cpu_state_ref.get_pc() == 0x0));
    }

    return 0;
}

int verify_jzjcoresoftware_loadbytetest() {
    //Load the loadbytetest program
    setup_emulator_with_program("loadbytetest");
    uint64_t expected_inst_count = 0;

    //This testcase assumes x30 starts at 0 (it forgot to set it)
    cpu_state_ref.set_r(30, 0);

    //Check the loop
    const uint32_t expected_x31[16] = {
        0x01, 0x23, 0x45, 0x67,
        0xFFFFFF89, 0xFFFFFFAB, 0xFFFFFFCD, 0xFFFFFFEF,
        0xFFFFFFFE, 0xFFFFFFDC, 0xFFFFFFBA, 0xFFFFFF98,
        0x76, 0x54, 0x32, 0x10
    };
    for (uint32_t i = 0; i < 16; ++i) {
        emulator.tick();//Execute the lb
        assert((emulator.get_inst_count() == ++expected_inst_count));
        assert((cpu_state_ref.get_pc() == 0x4));
        assert((cpu_state_ref.get_r(31) == expected_x31[i]));
        emulator.tick();//Execute the addi
        assert((emulator.get_inst_count() == ++expected_inst_count));
        assert((cpu_state_ref.get_pc() == 0x8));
        assert((cpu_state_ref.get_r(30) == (i + 1)));
        emulator.tick();//Execute the jalr
        assert((emulator.get_inst_count() == ++expected_inst_count));
        assert((cpu_state_ref.get_pc() == 0x0));
    }

    return 0;
}

int verify_jzjcoresoftware_luitest() {
    //Load the luitest program
    setup_emulator_with_program("luitest");
    uint64_t expected_inst_count = 0;

    //Check the two instructions
    emulator.tick();//Execute the lui
    assert((emulator.get_inst_count() == ++expected_inst_count));
    assert((cpu_state_ref.get_pc() == 0x4));
    assert((cpu_state_ref.get_r(31) == (703710 << 12)));
    emulator.run_until(0);//Execute the IRVE.EXIT
    assert((emulator.get_inst_count() == ++expected_inst_count));
    assert((cpu_state_ref.get_pc() == 0x4));

    return 0;
}

int verify_jzjcoresoftware_memoryreadtest() {
    //Load the memoryreadtest program
    setup_emulator_with_program("memoryreadtest");
    uint64_t expected_inst_count = 0;

    //Check the instruction
    emulator.run_until(1);//Execute the lw
    assert((emulator.get_inst_count() == ++expected_inst_count));
    assert((cpu_state_ref.get_pc() == 0x4));
    assert((cpu_state_ref.get_r(31) == 2863311530u));
    
    return 0;
}

int verify_jzjcoresoftware_memorywritetest() {
    //Load the memorywritetest program
    setup_emulator_with_program("memorywritetest");
    irve::internal::memory::memory_t& memory_ref = emulator.m_emulator_ptr->m_memory;
    uint64_t expected_inst_count = 0;

    //Check the instructions
    emulator.run_until(1);//Execute the addi
    assert((emulator.get_inst_count() == ++expected_inst_count));
    assert((cpu_state_ref.get_pc() == 0x4));
    assert((cpu_state_ref.get_r(30) == 26));
    emulator.tick();//Execute the sw
    assert((emulator.get_inst_count() == ++expected_inst_count));
    assert((cpu_state_ref.get_pc() == 0x8));
    assert((memory_ref.r(0x10, 0b010) == 26));
    emulator.tick();//Execute the lw
    assert((emulator.get_inst_count() == ++expected_inst_count));
    assert((cpu_state_ref.get_pc() == 0xC));
    assert((cpu_state_ref.get_r(31) == 26));
    emulator.tick();//Execute the IRVE.EXIT
    assert((emulator.get_inst_count() == ++expected_inst_count));
    assert((cpu_state_ref.get_pc() == 0xC));
    
    return 0;
}

//TODO the rest

/* Static Function Implementations */

static int verify_jzjcoresoftware_fibonacci(irve::emulator::emulator_t& emulator, irve::internal::cpu_state::cpu_state_t& cpu_state_ref) {
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
        emulator.tick();//Execute the jal or jalr
        assert((emulator.get_inst_count() == ++expected_inst_count));
        assert((cpu_state_ref.get_pc() == 0xC));
    }

    return 0;
}
