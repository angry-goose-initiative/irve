/* main.cpp
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * IRVE - The Inextensible RISC-V Emulator
 *
 * Entry point
 *
*/

/* Constants and Defines */

#define TESTFILES_DIR "testfiles/compiled/"

#define irvelog(...) irve::logging::log(__VA_ARGS__)

/* Includes */

#include "irve_public_api.h"

#include <cassert>
#include <iostream>
#include <string>

/* Function Implementations */

int main(int argc, char** argv) {
    if (irve::logging::logging_disabled()) {
        //Only print this
        std::cerr << "Starting IRVE" << std::endl;
        std::cerr << "Copyright (C) 2023 John Jekel and Nick Chan" << std::endl;
        std::cerr << "See the LICENSE file at the root of the project for licensing info." << std::endl;
        std::cerr << "libirve " << irve::about::get_version_string() << std::endl;
        std::cerr << "libirve built at " << irve::about::get_build_time_string() << " on " << irve::about::get_build_date_string() << std::endl;
        std::cerr << "irve executable built at " << __TIME__ << " on " << __DATE__ << std::endl;
        std::cerr << "Build system: " << irve::about::get_build_system_string() << std::endl;
        std::cerr << "Build from " << irve::about::get_build_host_string() << " for " << irve::about::get_compile_target_string() << std::endl;
        std::cerr << "Compiler: " << irve::about::get_compiler_string() << std::endl;
    }

    irvelog(0, "\x1b[1mStarting \x1b[94mIRVE\x1b[0m");
    irvelog(0, "\x1b[1m\x1b[94m ___ ______     _______ \x1b[0m");
    irvelog(0, "\x1b[1m\x1b[94m|_ _|  _ \\ \\   / / ____|\x1b[0m");
    irvelog(0, "\x1b[1m\x1b[94m | || |_) \\ \\ / /|  _|  \x1b[0m"); 
    irvelog(0, "\x1b[1m\x1b[94m | ||  _ < \\ V / | |___ \x1b[0m");
    irvelog(0, "\x1b[1m\x1b[94m|___|_| \\_\\ \\_/  |_____|\x1b[0m");
    irvelog(0, "");
    irvelog(0, "\x1b[1mThe Inextensible RISC-V Emulator\x1b[0m");
    irvelog(0, "\x1b[1mCopyright (C) 2023 \x1b[95mJohn Jekel\x1b[37m and \x1b[92mNick Chan\x1b[0m");
    irvelog(0, "\x1b[90mSee the LICENSE file at the root of the project for licensing info.\x1b[0m");
    irvelog(0, "");
    irvelog(0, "\x1b[90mlibirve %s\x1b[0m", irve::about::get_version_string());
    irvelog(0, "\x1b[90mlibirve built at %s on %s\x1b[0m", irve::about::get_build_time_string(), irve::about::get_build_date_string());
    irvelog(0, "\x1b[90mirve executable built at %s on %s\x1b[0m", __TIME__, __DATE__);
    irvelog(0, "\x1b[90mBuild System: %s\x1b[0m", irve::about::get_build_system_string());
    irvelog(0, "\x1b[90mBuilt from %s for %s\x1b[0m", irve::about::get_build_host_string(), irve::about::get_compile_target_string());
    irvelog(0, "\x1b[90mCompiler: %s\x1b[0m", irve::about::get_compiler_string());
    //TODO more info here
    irvelog(0, "------------------------------------------------------------------------");
    irvelog(0, "");
    irvelog(0, "");
   
    irvelog(0, "Initializing emulator...");
#undef INST_COUNT
#define INST_COUNT emulator.get_inst_count()
    irve::emulator::emulator_t emulator;

    if (argc < 2) {
        irvelog(0, "No memory image file specified. Starting with empty memory.");
    } else {
        assert(argc == 2 && "Too many arguments for now");//TODO remove this if we need in the future

        //Locate and the image file (guessing it if it is not a whole path for convenience)
        std::string mem_file = argv[1];
        //A testfile name rather than a path, so prepend the testfiles directory
        if (mem_file.find(TESTFILES_DIR) == std::string::npos) {
            mem_file = TESTFILES_DIR + mem_file;
        }

        irvelog(0, "Loading memory image from file \"%s\"", mem_file.c_str());
        irve::loader::load_verilog_32(emulator, mem_file.c_str());
    }

    while (emulator.tick());//Tick the emulator until we get an exit request

    irvelog(0, "\x1b[1mIRVE is shutting down. Bye bye!\x1b[0m");
    return 0;
}
