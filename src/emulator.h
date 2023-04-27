/* NAME//TODO
 * By: John Jekel
 *
 * TODO description
 *
*/

#ifndef EMULATOR_H
#define EMULATOR_H

/* Includes */

#include "cpu_state.h"
#include "Memory.hpp"

/* Constants And Defines */

//TODO

/* Types */

class emulator_t {
public:
    emulator_t();

    void tick();

    //TODO
private:
    cpu_state_t cpu_state;
    Memory memory;

    //TODO
};

/* Global Variables */

//TODO

/* Function/Class Declarations */

//TODO

#endif//EMULATOR_H
