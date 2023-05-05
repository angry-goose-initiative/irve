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

#define TESTFILES_DIR "testfiles/precompiled/"

/* Includes */

#include <cassert>
#include <iostream>
#include <fstream>
#include <string>

#include "emulator.h"

#define INST_COUNT emulator.get_inst_count()
#include "logging.h"

/* Static Function Declarations */

static void load_memory_image(emulator_t& emulator, const char* filename);

/* Function Implementations */

int main(int argc, char** argv) {

    emulator_t emulator;
    irvelog(0, "Starting IRVE");
    irvelog(1, "The Inextensible RISC-V Emulator");

    if (argc < 2) {
        irvelog(0, "No memory image file specified. Starting with empty memory.");
    } else {
        assert(argc == 2 && "Too many arguments for now");//TODO remove this if we need in the future
        load_memory_image(emulator, argv[1]);
    }

    //TESTING do an infinite loop instead
    for (uint32_t i = 0; i < 100; ++i) {
        emulator.tick();
    }

    irvelog(0, "IRVE is shutting down. Bye bye!");
    return 0;
}

/* Static Function Implementations */

//Verilog Hex format (32-bit words) + Backwards compatibility w/ previous file format
static void load_memory_image(emulator_t& emulator, const char* filename) {
    irvelog(0, "Loading memory image from file \"%s\"", filename);

    //Locate and open the image file 
    std::string mem_file = filename;
    //A testfile name rather than a path, so prepend the testfiles directory
    if (mem_file.find(TESTFILES_DIR) == std::string::npos) {
        mem_file = TESTFILES_DIR + mem_file;
    }
    std::fstream fin = std::fstream(mem_file);
    assert(fin && "Failed to open memory image file");

    //Read the file token by token
    uint32_t addr = 0;
    std::string token;
    while (fin >> token) {
        assert((token.length() != 0) && "This should never happen");
        if (token.at(0) == '@') {//New address
            std::string new_addr_str = token.substr(1);
            assert((new_addr_str.length() == 8) && "Memory image file is not formatted correctly (bad address)");
            addr = std::stoul(new_addr_str, nullptr, 16);
            assert(((addr % 4) == 0) && "Memory image file is not formatted correctly (address not word aligned)");
        } else {//New data word (32-bit, could be an instruction or data)
            assert((token.length() == 8) && "Memory image file is not formatted correctly");
            
            //The data word this token represents
            uint32_t data_word = 0;
            
            //For each hex digit in the token
            for (std::size_t i = 0; i < 8; ++i) {
                //Grab the digit
                char c = (char)std::toupper(token[i]);

                //Turn it into a numeric value
                uint32_t hex = 0;
                if ((c > 47) && (c < 58)) {
                    hex = c - 48;
                } else if ((c > 64) && (c < 71)) {
                    hex = c - 55;
                } else {
                    assert(0 && "Invalid character in memory image file");
                }

                //Or it into the data word in the correct position
                hex <<= (28 - (4 * i));
                data_word |= hex;
            }

            //Write the data word to memory and increment the address to the next word
            emulator.mem_write(addr, 0b010, data_word);
            addr += 4;
        }
    }
}
