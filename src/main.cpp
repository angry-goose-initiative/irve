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

/* Includes */

#include "irve_public_api.h"

#include <cassert>
#include <iostream>
#include <string>

//TODO get rid of these headers
#include "loader.h"
#include "emulator.h"
#include "common.h"

/* Function Implementations */

int main(int argc, char** argv) {
    if (irve::about::logging_disabled()) {
        //Only print this
        std::cerr << "Starting " << irve::about::get_version_string() << std::endl;
        std::cerr << "Copyright (C) 2023 John Jekel and Nick Chan" << std::endl;
        std::cerr << "See the LICENSE file at the root of the project for licensing info." << std::endl;
    }

    irve::log(0, "\x1b[1mStarting \x1b[94m%s\x1b[0m", irve::about::get_version_string());
    irve::log(0, "\x1b[1m\x1b[94m ___ ______     _______ \x1b[0m");
    irve::log(0, "\x1b[1m\x1b[94m|_ _|  _ \\ \\   / / ____|\x1b[0m");
    irve::log(0, "\x1b[1m\x1b[94m | || |_) \\ \\ / /|  _|  \x1b[0m"); 
    irve::log(0, "\x1b[1m\x1b[94m | ||  _ < \\ V / | |___ \x1b[0m");
    irve::log(0, "\x1b[1m\x1b[94m|___|_| \\_\\ \\_/  |_____|\x1b[0m");
    irve::log(0, "");
    irve::log(0, "\x1b[1mThe Inextensible RISC-V Emulator\x1b[0m");
    irve::log(0, "\x1b[1mCopyright (C) 2023 \x1b[95mJohn Jekel\x1b[37m and \x1b[92mNick Chan\x1b[0m");
    irve::log(0, "\x1b[90mSee the LICENSE file at the root of the project for licensing info.\x1b[0m");
    irve::log(0, "------------------------------------------------------------------------");
    irve::log(0, "");
    irve::log(0, "");
   
    irve::log(0, "Initializing emulator...");
#undef INST_COUNT
#define INST_COUNT emulator.get_inst_count()
    emulator_t emulator;

    if (argc < 2) {
        irve::log(0, "No memory image file specified. Starting with empty memory.");
    } else {
        assert(argc == 2 && "Too many arguments for now");//TODO remove this if we need in the future

        //Locate and the image file (guessing it if it is not a whole path for convenience)
        std::string mem_file = argv[1];
        //A testfile name rather than a path, so prepend the testfiles directory
        if (mem_file.find(TESTFILES_DIR) == std::string::npos) {
            mem_file = TESTFILES_DIR + mem_file;
        }

        irve::log(0, "Loading memory image from file \"%s\"", mem_file.c_str());
        irve::loader::load_verilog_32(emulator, mem_file.c_str());
    }

    while (emulator.tick());//Tick the emulator until we get an exit request

    irve::log(0, "\x1b[1mIRVE is shutting down. Bye bye!\x1b[0m");
    return 0;
}
