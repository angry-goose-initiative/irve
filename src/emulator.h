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
#include "Memory.h"

/* Constants And Defines */

//TODO

/* Types */

class emulator_t {
public:
    emulator_t();

    void tick();

    uint64_t get_inst_count() const;

private:
    uint32_t fetch();//TODO or should this be done in a seperate file?

    cpu_state_t cpu_state;
    Memory memory;

    //TODO
};

/* Global Variables */

//TODO

/* Function/Class Declarations */

//TODO

#endif//EMULATOR_H
