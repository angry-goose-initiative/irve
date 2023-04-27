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

#include "Memory.hpp"

/* Types */

//TODO

/* Variables */

//TODO

/* Static Function Declarations */

//TODO

/* Function Implementations */

int main() {
    
    Memory memory;

    // write word 0x11223344 to address 0
    memory.w(0, 0b010, 0x11223344);
    memory.p(0);
    // write halfword 0x9988 to address 1
    memory.w(1, 0b001, 0x9988);
    memory.p(0);

    // print the signed byte at address 1
    std::cout << std::hex << memory.r(1, 0b000) << std::endl;
    // print the unsigned byte at address 1
    std::cout << std::hex << memory.r(1, 0b100) << std::endl;

}

/* Static Function Implementations */

//TODO
