/* NAME//TODO
 * By: John Jekel
 *
 * TODO description
 *
*/

/* Constants And Defines */

//TODO

/* Includes */

#include <cassert>
#include <iostream>

#include "Memory.h"
#include "emulator.h"

#define INST_COUNT emulator.get_inst_count()
#include "logging.h"

/* Types */

//TODO

/* Variables */

//TODO

/* Static Function Declarations */

static void load_memory_image(emulator_t &emulator, const char *filename);

/* Function Implementations */

int main(int argc, char **argv) {

    // std::cout << "Memory testing" << std::endl;
    
    // Memory memory;

    // // write word 0x11223344 to address 0
    // memory.w(0, 0b010, 0x11223344);
    // memory.p(0);
    // // write halfword 0x9988 to address 1
    // memory.w(1, 0b001, 0x9988);
    // memory.p(0);

    // // print the signed byte at address 1
    // std::cout << std::hex << memory.r(1, 0b000) << std::endl;
    // // print the unsigned byte at address 1
    // std::cout << std::hex << memory.r(1, 0b100) << std::endl;


    //std::cout << "Emulator testing" << std::endl;

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
    for (uint32_t i = 0; i < 10; ++i) {
        emulator.tick();
    }

    irvelog(0, "IRVE is shutting down. Bye bye!");
    return 0;
}

/* Static Function Implementations */

static void load_memory_image(emulator_t &emulator, const char *filename) {
    irvelog(0, "Loading memory image from file \"%s\"", filename);

    //Read a file into the emulator byte-by-byte
    FILE *file = fopen(filename, "rb");
    assert((file != NULL) && "Failed to open memory image file");
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);
    assert((file_size >= 0) && "Failed to get file size");
    irvelog(1, "Memory image file size is %ld bytes", file_size);
    for (long i = 0; i < file_size; ++i) {
        int8_t byte = fgetc(file);
        emulator.mem_write_byte(i, byte);
    }
}
