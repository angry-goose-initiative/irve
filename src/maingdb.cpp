/**
 * @file    maingdb.cpp
 * @brief   IRVE - The Inextensible RISC-V Emulator (GDB Server Version)
 *
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  See the LICENSE file at the root of the project for licensing info.
 * 
 * Entry point
 *
*/

//TODO better code reuse with this and main.cpp

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include "irve_public_api.h"

#include <cassert>
#include <chrono>
#include <iostream>
#include <optional>
#include <string>

/* ------------------------------------------------------------------------------------------------
 * Constants/Defines
 * --------------------------------------------------------------------------------------------- */

#define PORT 12345

#define irvelog(...) irve::logging::log(__VA_ARGS__)
#define irvelog_always(...) irve::logging::log_always(__VA_ARGS__)

/* ------------------------------------------------------------------------------------------------
 * Static Function Declarations
 * --------------------------------------------------------------------------------------------- */

static void print_startup_message();

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

int main(int argc, const char* const* argv) {
    auto irve_boot_time = std::chrono::steady_clock::now();

    //srand(time(NULL));//Uncomment this to seed the random number generator that the emulator uses

    print_startup_message();

    if (irve::about::fuzzish_build()) {
        auto seed = time(NULL);
        srand(seed);
        irvelog_always(0, "Fuzzish Build: Set seed to %lu", seed);
    }

    irvelog_always(0, "Initializing emulator...");

    std::optional<irve::emulator::emulator_t> emulator;
    try {
        emulator.emplace(argc - 1, &(argv[1]));
    } catch (...) {
        irvelog_always(0, "Failed to initialize the emulator!");
        return 1;
    }

    auto init_time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - irve_boot_time).count();

    irvelog_always(0, "Initialized the emulator in %luus", init_time);

    irvelog_always(0, "Starting the IRVE GDB Server...");

    emulator->run_gdbserver(12345);

    irvelog_always(0, "\x1b[1mIRVE is shutting down. Bye bye!\x1b[0m");
    
    return 0;
}

/* ------------------------------------------------------------------------------------------------
 * Static Function Implementations
 * --------------------------------------------------------------------------------------------- */

static void print_startup_message() {
    irvelog_always(0, "\x1b[1mStarting \x1b[94mIRVE\x1b[0m (GDB Server Mode)");
    irvelog_always(0, "\x1b[1m\x1b[94m ___ ______     _______  \x1b[90m__\x1b[0m");
    irvelog_always(0, "\x1b[1m\x1b[94m|_ _|  _ \\ \\   / / ____| \x1b[90m\\ \\\x1b[0m");
    irvelog_always(0, "\x1b[1m\x1b[94m | || |_) \\ \\ / /|  _|    \x1b[90m\\ \\\x1b[0m");
    irvelog_always(0, "\x1b[1m\x1b[94m | ||  _ < \\ V / | |___   \x1b[90m/ /\x1b[0m");
    irvelog_always(0, "\x1b[1m\x1b[94m|___|_| \\_\\ \\_/  |_____| \x1b[90m/_/\x1b[0m");
    irvelog_always(0, "");
    irvelog_always(0, "\x1b[1mThe Inextensible RISC-V Emulator\x1b[0m");
    irvelog_always(0, "\x1b[1mCopyright (C) 2023-2024 \x1b[95mJohn Jekel\x1b[0m");
    irvelog_always(0, "\x1b[1mCopyright (C) 2023-2024 \x1b[92mNick Chan\x1b[0m");
    irvelog_always(0, "\x1b[1mCopyright (C) 2024      \x1b[96mSam Graham\x1b[0m");
    irvelog_always(0, "\x1b[90mSee the LICENSE file at the root of the project for licensing info.\x1b[0m");
    irvelog_always(0, "");
    irvelog_always(0, "\x1b[90mlibirve %s\x1b[0m", irve::about::get_version_string());
    irvelog_always(0, "\x1b[90mlibirve built at %s on %s\x1b[0m", irve::about::get_build_time_string(), irve::about::get_build_date_string());
    irvelog_always(0, "\x1b[90mirvegdb executable built at %s on %s\x1b[0m", __TIME__, __DATE__);
    irvelog(0, "\x1b[90mBuild System: %s\x1b[0m", irve::about::get_build_system_string());
    irvelog(0, "\x1b[90mBuilt from %s for %s\x1b[0m", irve::about::get_build_host_string(), irve::about::get_compile_target_string());
    irvelog(0, "\x1b[90mCompiler: %s\x1b[0m", irve::about::get_compiler_string());
    irvelog_always(0, "------------------------------------------------------------------------");
    irvelog_always(0, "");
    irvelog_always(0, "");
}
