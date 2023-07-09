/**
 * @file    main.cpp
 * @brief   IRVE - The Inextensible RISC-V Emulator
 * 
 * @copyright Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 * 
 * Entry point
 *
*/

//TODO better code reuse with this and maingdb.cpp

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

    auto execution_start_time = std::chrono::steady_clock::now();

    emulator->run_until(0);//Run the emulator until we get an exit request

    auto execution_time_us = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - execution_start_time).count();

    irvelog_always(0, "Emulation finished in %luus", execution_time_us);
    irvelog_always(0, "%lu instructions were executed", emulator->get_inst_count());
    auto average_ips = (((double)emulator->get_inst_count()) / ((double)execution_time_us)) * 1000000.0;
    irvelog_always(0, "Average of %f instructions per second (%fMHz)", average_ips, (average_ips / 1000000.0));

    irvelog_always(0, "\x1b[1mIRVE is shutting down. Bye bye!\x1b[0m");
    
    return 0;
}

/* ------------------------------------------------------------------------------------------------
 * Static Function Implementations
 * --------------------------------------------------------------------------------------------- */

static void print_startup_message() {
    irvelog_always(0, "\x1b[1mStarting \x1b[94mIRVE\x1b[0m");
    irvelog_always(0, "\x1b[1m\x1b[94m ___ ______     _______ \x1b[0m");
    irvelog_always(0, "\x1b[1m\x1b[94m|_ _|  _ \\ \\   / / ____|\x1b[0m");
    irvelog_always(0, "\x1b[1m\x1b[94m | || |_) \\ \\ / /|  _|  \x1b[0m"); 
    irvelog_always(0, "\x1b[1m\x1b[94m | ||  _ < \\ V / | |___ \x1b[0m");
    irvelog_always(0, "\x1b[1m\x1b[94m|___|_| \\_\\ \\_/  |_____|\x1b[0m");
    irvelog_always(0, "");
    irvelog_always(0, "\x1b[1mThe Inextensible RISC-V Emulator\x1b[0m");
    irvelog_always(0, "\x1b[1mCopyright (C) 2023 \x1b[95mJohn Jekel\x1b[37m and \x1b[92mNick Chan\x1b[0m");
    irvelog_always(0, "\x1b[90mSee the LICENSE file at the root of the project for licensing info.\x1b[0m");
    irvelog_always(0, "");
    irvelog_always(0, "\x1b[90mlibirve %s\x1b[0m", irve::about::get_version_string());
    irvelog_always(0, "\x1b[90mlibirve built at %s on %s\x1b[0m", irve::about::get_build_time_string(), irve::about::get_build_date_string());
    irvelog_always(0, "\x1b[90mirve executable built at %s on %s\x1b[0m", __TIME__, __DATE__);
    irvelog(0, "\x1b[90mBuild System: %s\x1b[0m", irve::about::get_build_system_string());
    irvelog(0, "\x1b[90mBuilt from %s for %s\x1b[0m", irve::about::get_build_host_string(), irve::about::get_compile_target_string());
    irvelog(0, "\x1b[90mCompiler: %s\x1b[0m", irve::about::get_compiler_string());
    irvelog_always(0, "------------------------------------------------------------------------");
    irvelog_always(0, "");
    irvelog_always(0, "");
}
