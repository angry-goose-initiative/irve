/* rv32esim.cpp
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Verifies the rv32esim rvsw single_file assembly program
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
#include <cstring>

/* Function Implementations */

//TODO also check CSRs, memory, etc

int verify_rv32esim() {
    //Load rv32esim program
    const char* file_name_ptr = "rvsw/compiled/src/single_file/asm/rv32esim.vhex8";
    irve::emulator::emulator_t emulator(1, &file_name_ptr);
    irve::internal::cpu_state::cpu_state_t& cpu_state_ref = emulator.m_emulator_ptr->m_cpu_state;
    uint64_t expected_inst_count = 0;

    //Check the initial instructions
    emulator.tick();//Execute the nop
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0x4);
    emulator.tick();//Execute the first li (lui)
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0x8);
    emulator.tick();//Execute the first li (addi)
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0xC);
    assert(cpu_state_ref.get_r(1) == 123456789);
    emulator.tick();//Execute the second li (addi)
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0x10);
    assert(cpu_state_ref.get_r(2) == 123);
    emulator.tick();//Execute the addi
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0x14);
    assert(cpu_state_ref.get_r(3) == (123 + 456));
    emulator.tick();//Execute the slti
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0x18);
    assert(cpu_state_ref.get_r(4) == 0);
    emulator.tick();//Execute the sltiu
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0x1C);
    assert(cpu_state_ref.get_r(5) == 0);
    emulator.tick();//Execute the lw
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0x20);
    assert(cpu_state_ref.get_r(6) == 0x00000013);
    emulator.tick();//Execute the j
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0x24);
    emulator.tick();//Execute the la (auipc)
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0x28);
    assert(cpu_state_ref.get_r(7) == 0x24);
    emulator.tick();//Execute the la (addi)
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0x2C);
    assert(cpu_state_ref.get_r(7) == 0x44);

    //Check the loop
    const char* string = "Hello World!\n";
    for (uint32_t i = 0; i < std::strlen(string); ++i) {
        assert(cpu_state_ref.get_pc() == 0x2C);
        emulator.tick();//Execute the lbu
        assert(emulator.get_inst_count() == ++expected_inst_count);
        assert(cpu_state_ref.get_pc() == 0x30);
        assert(cpu_state_ref.get_r(8) == (uint32_t)(string[i]));
        emulator.tick();//Execute the beqz (not taken)
        assert(emulator.get_inst_count() == ++expected_inst_count);
        assert(cpu_state_ref.get_pc() == 0x34);
        emulator.tick();//Execute the sb
        assert(emulator.get_inst_count() == ++expected_inst_count);
        assert(cpu_state_ref.get_pc() == 0x38);
        //TODO verify the physical memory debug std::string has this character now
        emulator.tick();//Execute the addi
        assert(emulator.get_inst_count() == ++expected_inst_count);
        assert(cpu_state_ref.get_pc() == 0x3C);
        assert(cpu_state_ref.get_r(7) == (0x44 + (i + 1)));
        emulator.tick();//Execute the j
        assert(emulator.get_inst_count() == ++expected_inst_count);
        assert(cpu_state_ref.get_pc() == 0x2C);
    }

    //End loop stuffs
    assert(cpu_state_ref.get_pc() == 0x2C);
    emulator.tick();//Execute the lbu
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0x30);
    assert(cpu_state_ref.get_r(8) == 0);
    emulator.tick();//Execute the beqz (this time it should be taken)
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0x40);
    emulator.tick();//Execute the IRVE.EXIT
    assert(emulator.get_inst_count() == ++expected_inst_count);
    assert(cpu_state_ref.get_pc() == 0x40);

    return 0;
}

int verify_atomics() {
    //Load atomics program
    const char* file_name_ptr = "rvsw/compiled/src/single_file/asm/atomics.vhex8";
    __attribute__((unused)) irve::emulator::emulator_t emulator(1, &file_name_ptr);
    __attribute__((unused)) irve::internal::cpu_state::cpu_state_t& cpu_state_ref =
        emulator.m_emulator_ptr->m_cpu_state;
    __attribute__((unused)) uint64_t expected_inst_count = 0;

    emulator.tick(); // la a0, my_words
    emulator.tick();
    if(cpu_state_ref.get_r(6) != 0x12345678) {
        emulator.tick();
    }
    assert(cpu_state_ref.get_r(6) == 0x12345678);

    // Testing load reserved and store conditional

    emulator.tick(); // lr.w t0, 0(a0)
    assert(cpu_state_ref.get_r(5) == 0x12345678);
    emulator.tick(); // lw t0, 4(a0)
    emulator.tick(); // sc.w t1, t0, 0(a0)
    assert(cpu_state_ref.get_r(6) == 0);
    emulator.tick(); // sc.w t1, t2, (a0)
    assert(cpu_state_ref.get_r(6) == 1);
    emulator.tick(); // lw t1, 0(a0)
    assert(cpu_state_ref.get_r(6) == 0x87654321);

    // Testing amo

    // swap
    emulator.tick();
    emulator.tick();
    emulator.tick();
    emulator.tick();
    emulator.tick();
    assert(cpu_state_ref.get_r(6) == 0xABCDEF01);
    assert(cpu_state_ref.get_r(7) == 0x87654321);

    // add
    emulator.tick();
    emulator.tick();
    emulator.tick();
    emulator.tick();
    emulator.tick();
    assert(cpu_state_ref.get_r(6) == 0x11111111);
    assert(cpu_state_ref.get_r(7) == 0x44444444);

    // and
    emulator.tick();
    emulator.tick();
    emulator.tick();
    emulator.tick();
    emulator.tick();
    assert(cpu_state_ref.get_r(6) == 0x11111111);
    assert(cpu_state_ref.get_r(7) == 0x11111111);

    // or
    emulator.tick();
    emulator.tick();
    emulator.tick();
    emulator.tick();
    emulator.tick();
    assert(cpu_state_ref.get_r(6) == 0x11111111);
    assert(cpu_state_ref.get_r(7) == 0x33333333);

    // xor
    emulator.tick();
    emulator.tick();
    emulator.tick();
    emulator.tick();
    emulator.tick();
    assert(cpu_state_ref.get_r(6) == 0x11111111);
    assert(cpu_state_ref.get_r(7) == 0x22222222);

    // max
    emulator.tick();
    emulator.tick();
    emulator.tick();
    emulator.tick();
    emulator.tick();
    assert(cpu_state_ref.get_r(6) == 0xFFFFFFFF);
    assert(cpu_state_ref.get_r(7) == 0x7FFFFFFF);

    // maxu
    emulator.tick();
    emulator.tick();
    emulator.tick();
    emulator.tick();
    emulator.tick();
    assert(cpu_state_ref.get_r(6) == 0xFFFFFFFF);
    assert(cpu_state_ref.get_r(7) == 0xFFFFFFFF);

    // min
    emulator.tick();
    emulator.tick();
    emulator.tick();
    emulator.tick();
    emulator.tick();
    assert(cpu_state_ref.get_r(6) == 0xFFFFFFFF);
    assert(cpu_state_ref.get_r(7) == 0xFFFFFFFF);

    // minu
    emulator.tick();
    emulator.tick();
    emulator.tick();
    emulator.tick();
    emulator.tick();
    assert(cpu_state_ref.get_r(6) == 0xFFFFFFFF);
    assert(cpu_state_ref.get_r(7) == 0x7FFFFFFF);

    return 0;
}
