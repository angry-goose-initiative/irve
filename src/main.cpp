/* NAME//TODO
 * By: John Jekel
 *
 * TODO description
 *
*/

/* Constants And Defines */

//TODO

/* Includes */

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

//TODO

/* Function Implementations */

int main() {

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
    //TODO more testing
    irvelog(0, "Hello world! %d", 123);//TESTING

    //TESTING
    for (uint32_t i = 0; i < 10; ++i) {
        emulator.tick();
    }


    irvelog(0, "IRVE is shutting down. Bye bye!");
    return 0;
}

/* Static Function Implementations */

//TODO
