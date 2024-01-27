/**
 * @file    jzjcoresoftware.cpp
 * @brief   Verifies that assembly programs originally from JZJCoreSoftware run as expected
 * 
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  See the LICENSE file at the root of the project for licensing info.
*/

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#define private public//Since we need to access internal emulator state for testing

#include "irve_public_api.h"
#include "emulator.h"
#include "cpu_state.h"
#include "csr.h"
#include "memory.h"

#include <cassert>
#include <cstdint>

/* ------------------------------------------------------------------------------------------------
 * Macros
 * --------------------------------------------------------------------------------------------- */

#define setup_emulator_with_program(program_name) \
    const char* file_name_ptr = "rvsw/compiled/src/single_file/asm/jzjcoresoftware/" program_name ".vhex8"; \
    irve::Emulator emulator(1, &file_name_ptr); \
    irve::internal::CpuState& cpu_state_ref = emulator.m_emulator_ptr->m_cpu_state;

/* ------------------------------------------------------------------------------------------------
 * Static Function Declarations
 * --------------------------------------------------------------------------------------------- */

static int verify_jzjcoresoftware_fibonacci(irve::Emulator& emulator, irve::internal::CpuState& cpu_state_ref);

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

//TODO also check CSRs, memory, etc

int verify_jzjcoresoftware_adding2() {
    //Load adding2 program
    setup_emulator_with_program("adding2");

    //This testcase assumes x31 starts at 0 (it forgot to set it)
    cpu_state_ref.set_r(31, 0);

    //Ensure all the adds are working
    for (uint32_t i = 0; i < 4096; ++i) {
        assert(emulator.get_inst_count() == i);//1 instruction per loop
        assert(cpu_state_ref.get_pc() == (i * 4));//4 bytes per instruction
        assert(cpu_state_ref.get_r(31) == (i * 2));//Expected result of adding2 each time
        emulator.tick();
    }

    //Ensure the program exits at instruction 4097
    emulator.run_until(0);
    assert(emulator.get_inst_count() == 4097);
    assert(cpu_state_ref.get_pc() == (4096 * 4));//Since 1st instruction is at 0
    assert(cpu_state_ref.get_r(31) == 8192);//Since last instruction is IRVE.EXIT

    return 0;
}

int verify_jzjcoresoftware_auipctest() {
    //Load the auipctest program
    setup_emulator_with_program("auipctest");
    emulator.run_until(0);
    
    //Checks things are as expected
    assert(emulator.get_inst_count() == 5);//3 nops, 1 auipc, 1 IRVE.EXIT
    assert(cpu_state_ref.get_pc() == 0x10);//5 4-byte instructions
    assert(cpu_state_ref.get_r(31) == ((1 << 12) + 0xC));//Expected result of auipc

    return 0;
}

int verify_jzjcoresoftware_bneandsubtest() {
    //Load the bneandsubtest program
    setup_emulator_with_program("bneandsubtest");
    uint64_t expected_inst_count = 0;
    
    //Checks things are as expected
    emulator.tick();
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0x4);
    assert(cpu_state_ref.get_r(31) == 1000);
    emulator.tick();
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0x8);
    assert(cpu_state_ref.get_r(31) == 1000);
    assert(cpu_state_ref.get_r(30) == 500);
    emulator.tick();
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0xc);
    assert(cpu_state_ref.get_r(31) == 1000);
    assert(cpu_state_ref.get_r(30) == 500);
    assert(cpu_state_ref.get_r(29) == 5);

    //Now we get to the loop
    uint32_t expected_x31 = 1000;
    while (expected_x31 >= 510) {
        emulator.tick();//Execute the sub
        expected_x31 -= 5;
        assert(emulator.get_inst_count() == ++expected_inst_count);
        assert(cpu_state_ref.get_pc() == 0x10);
        assert(cpu_state_ref.get_r(31) == expected_x31);
        emulator.tick();//Execute the bne
        assert(emulator.get_inst_count() == ++expected_inst_count);
        assert(cpu_state_ref.get_pc() == 0xc);
    }

    //On the last iteration, the bne should not be taken
    emulator.tick();//Execute the sub
    expected_x31 -= 5;
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0x10);
    assert(cpu_state_ref.get_r(31) == expected_x31);
    emulator.tick();//Execute the bne
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0x14);

    emulator.run_until(0);//Execute the IRVE.EXIT
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0x14);
    
    return 0;
}

int verify_jzjcoresoftware_callrettest() {
    //Load the callrettest program
    setup_emulator_with_program("callrettest");
    uint64_t expected_inst_count = 0;

    //This testcase assumes x31 starts at 0 (it forgot to set it)
    cpu_state_ref.set_r(31, 0);

    //Checks things are as expected
    emulator.tick();//Execute the addi
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0x4);
    assert(cpu_state_ref.get_r(31) == 10);
    emulator.tick();//Execute the jal (call)
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0xC);
    assert(cpu_state_ref.get_r(31) == 10);
    emulator.tick();//Execute the addi
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0x10);
    assert(cpu_state_ref.get_r(31) == 27);
    emulator.tick();//Execute the jalr (ret)
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0x8);
    assert(cpu_state_ref.get_r(31) == 27);
    emulator.run_until(0);//Execute the IRVE.EXIT
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0x8);
    assert(cpu_state_ref.get_r(31) == 27);
    
    return 0;
}

int verify_jzjcoresoftware_fenceecalltest() {
    //Load the fenceecalltest program
    setup_emulator_with_program("fenceecalltest");
    uint64_t expected_inst_count = 0;
    
    //This testcase assumes the trap vector starts at 0x4 (it forgot to set it)
    irve::internal::Csr& CSR_ref = emulator.m_emulator_ptr->m_CSR;
    assert((CSR_ref.implicit_read(irve::internal::Csr::Address::MTVEC) & 0xFFFFFFFC) == 0x4);

    //Checks things are as expected
    emulator.tick();//Execute the fence
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0x4);
    for (uint32_t i = 0; i < 100; ++i) {//Loop a few times (this program runs forever)
        for (uint32_t j = 0; j < 7; ++j) {
            emulator.tick();//Execute an addi
            assert(emulator.get_inst_count() == ++expected_inst_count);
            assert((cpu_state_ref.get_pc() == (0x8 + (j * 4))));
        }
        emulator.tick();//Execute the ecall
        assert(emulator.get_inst_count() == expected_inst_count);//Should NOT have been retired
        assert(cpu_state_ref.get_pc() == 0x4);//Since this is where the exception handler is (based on IRVE's hardcoded MTVEC)
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
        assert(emulator.get_inst_count() == ++expected_inst_count);
        assert(cpu_state_ref.get_pc() == 0x4);
        assert(cpu_state_ref.get_r(31) == expected_x31[i]);
        emulator.tick();//Execute the addi
        assert(emulator.get_inst_count() == ++expected_inst_count);
        assert(cpu_state_ref.get_pc() == 0x8);
        assert(cpu_state_ref.get_r(30) == (i + 1));
        emulator.tick();//Execute the jalr
        assert(emulator.get_inst_count() == ++expected_inst_count);
        assert(cpu_state_ref.get_pc() == 0x0);
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
        assert(emulator.get_inst_count() == ++expected_inst_count);
        assert(cpu_state_ref.get_pc() == 0x4);
        assert(cpu_state_ref.get_r(31) == expected_x31[i]);
        emulator.tick();//Execute the addi
        assert(emulator.get_inst_count() == ++expected_inst_count);
        assert(cpu_state_ref.get_pc() == 0x8);
        assert(cpu_state_ref.get_r(30) == ((i + 1) * 2));
        emulator.tick();//Execute the jalr
        assert(emulator.get_inst_count() == ++expected_inst_count);
        assert(cpu_state_ref.get_pc() == 0x0);
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
        assert(emulator.get_inst_count() == ++expected_inst_count);
        assert(cpu_state_ref.get_pc() == 0x4);
        assert(cpu_state_ref.get_r(31) == expected_x31[i]);
        emulator.tick();//Execute the addi
        assert(emulator.get_inst_count() == ++expected_inst_count);
        assert(cpu_state_ref.get_pc() == 0x8);
        assert(cpu_state_ref.get_r(30) == ((i + 1) * 2));
        emulator.tick();//Execute the jalr
        assert(emulator.get_inst_count() == ++expected_inst_count);
        assert(cpu_state_ref.get_pc() == 0x0);
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
        assert(emulator.get_inst_count() == ++expected_inst_count);
        assert(cpu_state_ref.get_pc() == 0x4);
        assert(cpu_state_ref.get_r(31) == expected_x31[i]);
        emulator.tick();//Execute the addi
        assert(emulator.get_inst_count() == ++expected_inst_count);
        assert(cpu_state_ref.get_pc() == 0x8);
        assert(cpu_state_ref.get_r(30) == (i + 1));
        emulator.tick();//Execute the jalr
        assert(emulator.get_inst_count() == ++expected_inst_count);
        assert(cpu_state_ref.get_pc() == 0x0);
    }

    return 0;
}

int verify_jzjcoresoftware_luitest() {
    //Load the luitest program
    setup_emulator_with_program("luitest");
    uint64_t expected_inst_count = 0;

    //Check the two instructions
    emulator.tick();//Execute the lui
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0x4);
    assert(cpu_state_ref.get_r(31) == (703710 << 12));
    emulator.run_until(0);//Execute the IRVE.EXIT
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0x4);

    return 0;
}

int verify_jzjcoresoftware_memoryreadtest() {
    //Load the memoryreadtest program
    setup_emulator_with_program("memoryreadtest");
    uint64_t expected_inst_count = 0;

    //Check the instruction
    emulator.run_until(1);//Execute the lw
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0x4);
    assert(cpu_state_ref.get_r(31) == 2863311530u);
    
    return 0;
}

int verify_jzjcoresoftware_memorywritetest() {
    //Load the memorywritetest program
    setup_emulator_with_program("memorywritetest");
    irve::internal::Memory& memory_ref = emulator.m_emulator_ptr->m_memory;
    uint64_t expected_inst_count = 0;

    //Check the instructions
    emulator.run_until(1);//Execute the addi
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0x4);
    assert(cpu_state_ref.get_r(30) == 26);
    emulator.tick();//Execute the sw
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0x8);
    assert(memory_ref.load(0x10, 0b010) == 26);
    emulator.tick();//Execute the lw
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0xC);
    assert(cpu_state_ref.get_r(31) == 26);
    emulator.tick();//Execute the IRVE.EXIT
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0xC);
    
    return 0;
}

int verify_jzjcoresoftware_nop() {
    //Load nop program
    setup_emulator_with_program("nop");
    uint64_t expected_inst_count = 0;
    uint32_t expected_x31 = 0;

    //This testcase assumes x31 starts at 0 (it forgot to set it)
    cpu_state_ref.set_r(31, 0);

    //Ensure all the adds are working
    for (uint32_t i = 0; i < 1024; ++i) {
        //Execute addi
        emulator.tick();
        expected_x31 += 2;
        assert(emulator.get_inst_count() == ++expected_inst_count);
        assert(cpu_state_ref.get_pc() == ((i * 0x10) + 4));
        assert(cpu_state_ref.get_r(31) == expected_x31);
        
        //Execute three nops
        emulator.tick();
        emulator.tick();
        emulator.tick();
        expected_inst_count += 3;
        assert(emulator.get_inst_count() == expected_inst_count);
    }

    //Ensure the program exits at instruction 4097
    emulator.run_until(0);
    assert(emulator.get_inst_count() == 4097);
    assert(cpu_state_ref.get_pc() == (4096 * 4));//Since 1st instruction is at 0
    assert(cpu_state_ref.get_r(31) == 2048);//Since last instruction is IRVE.EXIT

    return 0;
}

int verify_jzjcoresoftware_sbtest() {
    //Load the sbtest program
    setup_emulator_with_program("sbtest");
    irve::internal::Memory& memory_ref = emulator.m_emulator_ptr->m_memory;
    uint64_t expected_inst_count = 0;

    //Check the instructions
    assert(memory_ref.load(0xC, 0b010) == 0x89ABCDEF);
    emulator.tick();//Execute the sb
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0x4);
    assert(memory_ref.load(0xC, 0b000) == 0x00);
    assert(memory_ref.load(0xC, 0b010) == 0x89ABCD00);
    emulator.tick();//Execute the lw
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0x8);
    assert(cpu_state_ref.get_r(31) == 0x89ABCD00);
    emulator.tick();//Execute the IRVE.EXIT
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0x8);

    return 0;
}

int verify_jzjcoresoftware_sbtest2() {
    //Load the sbtest2 program
    setup_emulator_with_program("sbtest2");
    irve::internal::Memory& memory_ref = emulator.m_emulator_ptr->m_memory;
    uint64_t expected_inst_count = 0;

    //Check the instructions
    assert(memory_ref.load(0xC, 0b010) == 0x89ABCDEF);
    emulator.tick();//Execute the sb
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0x4);
    assert(memory_ref.load(0xD, 0b000) == 0x00);
    assert(memory_ref.load(0xC, 0b010) == 0x89AB00EF);
    emulator.tick();//Execute the lw
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0x8);
    assert(cpu_state_ref.get_r(31) == 0x89AB00EF);
    emulator.tick();//Execute the IRVE.EXIT
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0x8);

    return 0;
}

int verify_jzjcoresoftware_shtest() {
    //Load the shtest program
    setup_emulator_with_program("shtest");
    irve::internal::Memory& memory_ref = emulator.m_emulator_ptr->m_memory;
    uint64_t expected_inst_count = 0;

    //Check the instructions
    assert(memory_ref.load(0x10, 0b010) == 0x89abcdef);
    emulator.tick();//Execute the sh
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0x4);
    assert(memory_ref.load(0x10, 0b001) == 0x0000);
    assert(memory_ref.load(0x10, 0b010) == 0x89AB0000);
    emulator.tick();//Execute the first lhu 
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0x8);
    assert(cpu_state_ref.get_r(31) == 0x00000000);
    emulator.tick();//Execute the second lhu 
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0xC);
    assert(cpu_state_ref.get_r(31) == 0x000089AB);
    emulator.tick();//Execute the IRVE.EXIT
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0xC);

    return 0;
}

int verify_jzjcoresoftware_sllisrliblttest() {
    //Load the sllisrliblttest program
    setup_emulator_with_program("sllisrliblttest");
    uint64_t expected_inst_count = 0;
    uint32_t expected_x29 = 0;
    const uint32_t expected_x30 = 15;
    uint32_t expected_x31 = 1;

    //Check the initial instructions
    emulator.tick();//Execute the first addi
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0x4);
    assert(cpu_state_ref.get_r(29) == expected_x29);
    emulator.tick();//Execute the second addi
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0x8);
    assert(cpu_state_ref.get_r(30) == expected_x30);
    emulator.tick();//Execute the third addi
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0xC);
    assert(cpu_state_ref.get_r(31) == expected_x31);

    //Test the loops
    for (uint32_t i = 0; i < 100; ++i) {//100 iterations of outer loop should be fine (it runs infinitely)
        for (uint32_t j = 0; j < 15; ++j) {//15 iterations of both inner loops
            assert(cpu_state_ref.get_pc() == 0xC);
            emulator.tick();//Execute the slli 
            expected_x31 = expected_x31 << 1;
            assert(emulator.get_inst_count() == ++expected_inst_count);
            assert(cpu_state_ref.get_pc() == 0x10);
            assert(cpu_state_ref.get_r(31) == expected_x31);
            emulator.tick();//Execute the addi
            expected_x29 += 1;
            assert(emulator.get_inst_count() == ++expected_inst_count);
            assert(cpu_state_ref.get_pc() == 0x14);
            assert(cpu_state_ref.get_r(29) == expected_x29);
            emulator.tick();//Execute the blt
            assert(emulator.get_inst_count() == ++expected_inst_count);
        }
        assert(cpu_state_ref.get_pc() == 0x18);

        emulator.tick();//Execute the addi in between the two inner loops
        expected_x29 = 0;
        assert(emulator.get_inst_count() == ++expected_inst_count);
        assert(cpu_state_ref.get_pc() == 0x1C);
        assert(cpu_state_ref.get_r(29) == expected_x29);

        for (uint32_t j = 0; j < 15; ++j) {//15 iterations of both inner loops
            assert(cpu_state_ref.get_pc() == 0x1C);
            emulator.tick();//Execute the srli 
            expected_x31 = expected_x31 >> 1;//Logical shift
            assert(emulator.get_inst_count() == ++expected_inst_count);
            assert(cpu_state_ref.get_pc() == 0x20);
            assert(cpu_state_ref.get_r(31) == expected_x31);
            emulator.tick();//Execute the addi
            expected_x29 += 1;
            assert(emulator.get_inst_count() == ++expected_inst_count);
            assert(cpu_state_ref.get_pc() == 0x24);
            assert(cpu_state_ref.get_r(29) == expected_x29);
            emulator.tick();//Execute the blt
            assert(emulator.get_inst_count() == ++expected_inst_count);
        }
        assert(cpu_state_ref.get_pc() == 0x28);

        //Tail of the main loop
        emulator.tick();//Execute the addi
        expected_x29 = 0;
        assert(emulator.get_inst_count() == ++expected_inst_count);
        assert(cpu_state_ref.get_pc() == 0x2C);
        assert(cpu_state_ref.get_r(29) == expected_x29);
        emulator.tick();//Execute the jalr
        assert(emulator.get_inst_count() == ++expected_inst_count);
        assert(cpu_state_ref.get_pc() == 0xC);
    }

    return 0;
}

int verify_jzjcoresoftware_sllsrlblttest() {
    //Load the sllsrlblttest program
    setup_emulator_with_program("sllsrlblttest");
    uint64_t expected_inst_count = 0;
    const uint32_t expected_x28 = 1;
    uint32_t expected_x29 = 0;
    const uint32_t expected_x30 = 15;
    uint32_t expected_x31 = 1;

    //Check the initial instructions
    emulator.tick();//Execute the first addi
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0x4);
    assert(cpu_state_ref.get_r(28) == expected_x28);
    emulator.tick();//Execute the second addi
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0x8);
    assert(cpu_state_ref.get_r(29) == expected_x29);
    emulator.tick();//Execute the third addi
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0xC);
    assert(cpu_state_ref.get_r(30) == expected_x30);
    emulator.tick();//Execute the fourth addi
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0x10);
    assert(cpu_state_ref.get_r(31) == expected_x31);

    //Test the loops
    for (uint32_t i = 0; i < 100; ++i) {//100 iterations of outer loop should be fine (it runs infinitely)
        for (uint32_t j = 0; j < 15; ++j) {//15 iterations of both inner loops
            assert(cpu_state_ref.get_pc() == 0x10);
            emulator.tick();//Execute the sll
            expected_x31 = expected_x31 << 1;
            assert(emulator.get_inst_count() == ++expected_inst_count);
            assert(cpu_state_ref.get_pc() == 0x14);
            assert(cpu_state_ref.get_r(31) == expected_x31);
            emulator.tick();//Execute the addi
            expected_x29 += 1;
            assert(emulator.get_inst_count() == ++expected_inst_count);
            assert(cpu_state_ref.get_pc() == 0x18);
            assert(cpu_state_ref.get_r(29) == expected_x29);
            emulator.tick();//Execute the blt
            assert(emulator.get_inst_count() == ++expected_inst_count);
        }
        assert(cpu_state_ref.get_pc() == 0x1C);

        emulator.tick();//Execute the addi in between the two inner loops
        expected_x29 = 0;
        assert(emulator.get_inst_count() == ++expected_inst_count);
        assert(cpu_state_ref.get_pc() == 0x20);
        assert(cpu_state_ref.get_r(29) == expected_x29);

        for (uint32_t j = 0; j < 15; ++j) {//15 iterations of both inner loops
            assert(cpu_state_ref.get_pc() == 0x20);
            emulator.tick();//Execute the srl
            expected_x31 = expected_x31 >> 1;//Logical shift
            assert(emulator.get_inst_count() == ++expected_inst_count);
            assert(cpu_state_ref.get_pc() == 0x24);
            assert(cpu_state_ref.get_r(31) == expected_x31);
            emulator.tick();//Execute the addi
            expected_x29 += 1;
            assert(emulator.get_inst_count() == ++expected_inst_count);
            assert(cpu_state_ref.get_pc() == 0x28);
            assert(cpu_state_ref.get_r(29) == expected_x29);
            emulator.tick();//Execute the blt
            assert(emulator.get_inst_count() == ++expected_inst_count);
        }
        assert(cpu_state_ref.get_pc() == 0x2C);

        //Tail of the main loop
        emulator.tick();//Execute the addi
        expected_x29 = 0;
        assert(emulator.get_inst_count() == ++expected_inst_count);
        assert(cpu_state_ref.get_pc() == 0x30);
        assert(cpu_state_ref.get_r(29) == expected_x29);
        emulator.tick();//Execute the jalr
        assert(emulator.get_inst_count() == ++expected_inst_count);
        assert(cpu_state_ref.get_pc() == 0x10);
    }

    return 0;
}

int verify_jzjcoresoftware_testingfunctions() {
    //Load the testingfunctions program
    setup_emulator_with_program("testingfunctions");
    uint64_t expected_inst_count = 0;

    //Check the instructions
    emulator.tick();//Execute the first addi
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0x4);
    assert(cpu_state_ref.get_r(31) == 10);
    emulator.tick();//Execute the jal
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0xC);
    assert(cpu_state_ref.get_r(1) == 0x8);
    emulator.tick();//Execute the second addi
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0x10);
    assert(cpu_state_ref.get_r(31) == 27);
    emulator.tick();//Execute the jalr
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0x8);
    emulator.tick();//Execute the IRVE.EXIT
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0x8);

    return 0;
}

int verify_jzjcoresoftware_uncondjumptest() {
    //Load the uncondjumptest program
    setup_emulator_with_program("uncondjumptest");
    uint64_t expected_inst_count = 0;
    uint32_t expected_x31 = 0;

    //This testcase assumes x31 starts at 0 (it forgot to set it)
    cpu_state_ref.set_r(31, 0);

    //Check the instructions in the loop
    for (uint32_t i = 0; i < 100; ++i) {
        emulator.tick();//Execute the addi
        expected_x31 += 2;
        assert(emulator.get_inst_count() == ++expected_inst_count);
        assert(cpu_state_ref.get_pc() == 0x4);
        assert(cpu_state_ref.get_r(31) == expected_x31);
        emulator.tick();//Execute the jalr
        assert(emulator.get_inst_count() == ++expected_inst_count);
        assert(cpu_state_ref.get_pc() == 0x0);
    }

    return 0;
}

int verify_jzjcoresoftware_uncondjumptest2() {
    //Load the uncondjumptest2 program
    setup_emulator_with_program("uncondjumptest2");
    uint64_t expected_inst_count = 0;

    //Check the instructions in the loop
    for (uint32_t i = 0; i < 100; ++i) {
        emulator.tick();//Execute the addi
        assert(emulator.get_inst_count() == ++expected_inst_count);
        assert(cpu_state_ref.get_pc() == 0x4);
        assert(cpu_state_ref.get_r(31) == 2);
        emulator.tick();//Execute the jalr
        assert(emulator.get_inst_count() == ++expected_inst_count);
        assert(cpu_state_ref.get_pc() == 0x0);
        assert(cpu_state_ref.get_r(31) == 8);
    }

    return 0;
}

int verify_jzjcoresoftware_xoritoggle() {
    //Load the xoritoggle program
    setup_emulator_with_program("xoritoggle");
    uint64_t expected_inst_count = 0;
    uint32_t expected_x31 = 0;

    //This testcase assumes x31 starts at 0 (it forgot to set it)
    cpu_state_ref.set_r(31, 0);

    //Check the instructions in the loop
    for (uint32_t i = 0; i < 100; ++i) {
        emulator.tick();//Execute the xori
        expected_x31 ^= 0xFFFFFFFF;
        assert(emulator.get_inst_count() == ++expected_inst_count);
        assert(cpu_state_ref.get_pc() == 0x4);
        assert(cpu_state_ref.get_r(31) == expected_x31);
        emulator.tick();//Execute the jalr
        assert(emulator.get_inst_count() == ++expected_inst_count);
        assert(cpu_state_ref.get_pc() == 0x0);
    }

    return 0;
}

/* ------------------------------------------------------------------------------------------------
 * Static Function Implementations
 * --------------------------------------------------------------------------------------------- */

static int verify_jzjcoresoftware_fibonacci(irve::Emulator& emulator, irve::internal::CpuState& cpu_state_ref) {
    uint64_t expected_inst_count = 0;
    uint32_t expected_x29 = 0;
    uint32_t expected_x30 = 1;
    uint32_t expected_x31 = 0;

    //Checks things are as expected at the start
    emulator.tick();//Execute the first addi
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0x4);
    assert(cpu_state_ref.get_r(29) == expected_x29);
    emulator.tick();//Execute the second addi
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0x8);
    assert(cpu_state_ref.get_r(30) == expected_x30);
    emulator.tick();//Execute the third addi
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0xC);
    assert(cpu_state_ref.get_r(31) == expected_x31);
    
    //In the loop
    for (uint32_t i = 0; i < 47; ++i) {//Max fibonacci number that can fit in a 32 bit register is the 47th
        emulator.tick();//Execute the add
        expected_x31 = expected_x29 + expected_x30;
        assert(emulator.get_inst_count() == ++expected_inst_count);
        assert(cpu_state_ref.get_pc() == 0x10);
        assert(cpu_state_ref.get_r(31) == expected_x31);
        emulator.tick();//Execute the first addi
        expected_x29 = expected_x30;
        assert(emulator.get_inst_count() == ++expected_inst_count);
        assert(cpu_state_ref.get_pc() == 0x14);
        assert(cpu_state_ref.get_r(29) == expected_x29);
        emulator.tick();//Execute the second addi
        expected_x30 = expected_x31;
        assert(emulator.get_inst_count() == ++expected_inst_count);
        assert(cpu_state_ref.get_pc() == 0x18);
        assert(cpu_state_ref.get_r(30) == expected_x30);
        emulator.tick();//Execute the jal or jalr
        assert(emulator.get_inst_count() == ++expected_inst_count);
        assert(cpu_state_ref.get_pc() == 0xC);
    }

    return 0;
}
