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

    int8_t mem_read_byte(uint32_t addr) const;
    void mem_write_byte(uint32_t addr, int8_t data);

private:
    uint32_t fetch() const;

    cpu_state_t m_cpu_state;
    Memory m_memory;

    //TODO
};

/* Global Variables */

//TODO

/* Function/Class Declarations */

//TODO

#endif//EMULATOR_H
