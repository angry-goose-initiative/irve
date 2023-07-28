/**
 * @file    riscv_arch_tester.cpp
 * @brief   TODO
 * 
 * @copyright Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 * 
 * TODO longer description
 *
*/

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#define private public

#include "irve_public_api.h"

#include "common.h"
#include "emulator.h"
#include "memory.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <ctime>

/* ------------------------------------------------------------------------------------------------
 * Constants/Defines
 * --------------------------------------------------------------------------------------------- */

#define irvelog(...) irve::logging::log(__VA_ARGS__)
#define irvelog_always(...) irve::logging::log_always(__VA_ARGS__)

/* ------------------------------------------------------------------------------------------------
 * Type/Class Declarations
 * --------------------------------------------------------------------------------------------- */

//TODO

/* ------------------------------------------------------------------------------------------------
 * Static Variables
 * --------------------------------------------------------------------------------------------- */

//TODO

/* ------------------------------------------------------------------------------------------------
 * Static Function Declarations
 * --------------------------------------------------------------------------------------------- */

//TODO

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

int main(int argc, const char* const* argv) {
    //Need executable name, start signature address, end signature address, signature destination file, and at least one file to load
    assert((argc >= 5) && "Bad args");

    if (irve::about::fuzzish_build()) {
        auto seed = time(NULL);
        srand(seed);
        irvelog_always(0, "Fuzzish Build: Set seed to %lu", seed);
    }

    uint32_t signature_start_addr_inclusive = strtoll(argv[1], nullptr, 0);
    uint32_t signature_end_addr_exclusive   = strtoll(argv[2], nullptr, 0);
    const char* signature_filename = argv[3];
    int num_files_to_load = argc - 4;
    const char* const* file_list = &(argv[4]);

    irvelog_always(0, "Signature start inclusive:  0x%08X", signature_start_addr_inclusive);
    irvelog_always(0, "Signature end exclusive:    0x%08X", signature_end_addr_exclusive);
    assert(signature_start_addr_inclusive < signature_end_addr_exclusive);
    assert((signature_start_addr_inclusive % 4) == 0);
    assert((signature_end_addr_exclusive % 4) == 0);

    irvelog_always(0, "Signature destination file: \"%s\"", signature_filename);

    irvelog_always(0, "Initializing emulator...");
    irve::emulator::emulator_t emulator(num_files_to_load, file_list);
    irvelog_always(0, "Done!");

    irvelog_always(0, "Running test...");
    emulator.run_until(0);
    irvelog_always(0, "Done!");

    irvelog_always(0, "Dumping signature to \"%s\"...", signature_filename);
    //https://github.com/riscv-non-isa/riscv-arch-test/blob/main/spec/TestFormatSpec.adoc#the-test-signature
    FILE* signature_file = fopen(signature_filename, "w");
    irve::internal::memory::memory_t& memory_ref = emulator.m_emulator_ptr->m_memory;
    //We are guaranteed 4 byte alignment which is nice :)
    for (uint32_t addr = signature_start_addr_inclusive; addr < signature_end_addr_exclusive; addr += 4) {
        uint32_t signature_word = memory_ref.load(addr, 0b010).u;
        fprintf(signature_file, "%08x\n", signature_word);//This should match the way Spike outputs things (lowercase)
    }
    fclose(signature_file);
    irvelog_always(0, "Done!");

    return 0;
}

/* ------------------------------------------------------------------------------------------------
 * Static Function Implementations
 * --------------------------------------------------------------------------------------------- */

//TODO
