/**
 * @file    elf.cpp
 * @brief   Tests ELF parsing against other binary formats on RVSW programs
 * 
 * @copyright
 *  Copyright (C) 2024 John Jekel\n
 *  See the LICENSE file at the root of the project for licensing info.
*/

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#define private public//Since we need to access internal emulator state for testing (okay to do in unit tests)

#include "irve_public_api.h"
#include "emulator.h"
#include "memory.h"
#include "memory_map.h"

#undef NDEBUG//Asserts should work even in release mode for tests
#include <cassert>
#include <cstdint>
#include <cstring>

/* ------------------------------------------------------------------------------------------------
 * Constants
 * --------------------------------------------------------------------------------------------- */

#define NUM_PROGRAMS 3

const char* PROGRAM_LIST[NUM_PROGRAMS] = {
    "asm/rv32esim",
    "cxx/hello_cxx",
    "c/hello_world"
};

const char* RVSW_BASE = "rvsw/compiled/src/single_file/";

/* ------------------------------------------------------------------------------------------------
 * Static Function Declarations
 * --------------------------------------------------------------------------------------------- */

static void test_program(const char* file_name);

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

int verify_elf() {
    for (uint32_t i = 0; i < NUM_PROGRAMS; ++i) {
        test_program(PROGRAM_LIST[i]);
    }
    return 0;
}

/* ------------------------------------------------------------------------------------------------
 * Static Function Implementations
 * --------------------------------------------------------------------------------------------- */

static void test_program(const char* file_name) {
    std::string file_base = RVSW_BASE;
    file_base += file_name;

    std::string elf_path        = file_base + ".elf";
    std::string vhex8_path      = file_base + ".vhex8";
    const char* elf_path_cstr   = elf_path.c_str();
    const char* vhex8_path_cstr = vhex8_path.c_str();

    irve::emulator::emulator_t elf_emulator(0, NULL);
    irve::emulator::emulator_t vhex8_emulator(0, NULL);
    irve::internal::Memory& elf_memory_ref = elf_emulator.m_emulator_ptr->m_memory;
    irve::internal::Memory& vhex8_memory_ref = vhex8_emulator.m_emulator_ptr->m_memory;

    //Zero out all memory so we can compare it without worrying about garbage data
    for (uint32_t i = MEM_MAP_REGION_START_USER_RAM; i < MEM_MAP_REGION_END_USER_RAM; ++i) {
        elf_memory_ref.store(i, 0b000, 0);//One byte at a time
        vhex8_memory_ref.store(i, 0b000, 0);//One byte at a time
    }

    //Load the program (both ELF and vhex8)
    elf_memory_ref.load_memory_image_files(1, &elf_path_cstr);
    vhex8_memory_ref.load_memory_image_files(1, &vhex8_path_cstr);

    //Check that the memory is the same
    for (uint32_t i = MEM_MAP_REGION_START_USER_RAM; i < MEM_MAP_REGION_END_USER_RAM; ++i) {
        assert(elf_memory_ref.load(i, 0b000) == vhex8_memory_ref.load(i, 0b000));
    }
}
