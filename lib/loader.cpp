/* loader.cpp
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Code for loading RISC-V binaries into memory
 *
*/

/* Constants And Defines */

//TODO

/* Includes */

#include "loader.h"

#include "emulator.h"

#define INST_COUNT 0
#include "logging.h"

#include <fstream>
#include <string>

using namespace irve::internal;

/* Function Implementations */

//Verilog Hex format (32-bit words) + Backwards compatibility w/ previous file format
void loader::load_verilog_32(emulator::emulator_t& emulator, const char* filename) {
    //Open the image file
    std::fstream fin = std::fstream(filename);
    assert(fin && "Failed to open memory image file");

    //Read the file token by token
    word_t addr = 0;
    std::string token;
    while (fin >> token) {
        assert((token.length() != 0) && "This should never happen");
        if (token.at(0) == '@') {//New address (ASSUMING 32-BIT WORDS)
            std::string new_addr_str = token.substr(1);
            assert((new_addr_str.length() == 8) && "Memory image file is not formatted correctly (bad address)");
            addr.u = std::stoul(new_addr_str, nullptr, 16);
            addr *= 4;//This is a word address, not a byte address, so multiply by 4
            assert(((addr.u % 4) == 0) && "Memory image file is not formatted correctly (address not word aligned)");//TODO as it turns out, this is not a requirement
        } else {//New data word (32-bit, could be an instruction or data)
            if (token.length() != 8) {
                irvelog(1, "Warning: 32-bit Verilog image file is not formatted correctly (data word is not 8 characters long). This is likely an objcopy bug. Continuing anyway...");
            }
            
            //The data word this token represents
            word_t data_word;
            data_word.u = std::stoul(token, nullptr, 16);//If the weird objcopy bug happens, stoul will make the missing nibbles 0

            //Write the data word to memory and increment the address to the next word
            emulator.mem_write(addr, 0b010, data_word);
            addr += 4;
        }
    }
}
