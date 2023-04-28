/* NAME//TODO
 * By: John Jekel
 *
 * TODO description
 *
*/

/* Constants And Defines */

//TODO

/* Includes */

#include "emulator.h"

#include <cstdint>

#include "decode.h"

#define INST_COUNT this->get_inst_count()
#include "logging.h"

/* Types */

//TODO

/* Variables */

//TODO

/* Static Function Declarations */

//TODO

/* Function Implementations */

emulator_t::emulator_t(): m_cpu_state(), m_memory() {
    irvelog(0, "Created new emulator instance");
}

void emulator_t::tick() {
    this->m_cpu_state.increment_inst_count();
    irvelog(0, "Tick begins");

    uint32_t inst = this->fetch();

    irvelog(1, "Decoding instruction 0x%08X", inst);
    decoded_inst_t decoded_inst(inst);
    decoded_inst.log(2, this->get_inst_count());

    irvelog(1, "TODO execute instruction");

    irvelog(1, "TODO handle interrupts");

    irvelog(0, "Tick ends");
}

uint64_t emulator_t::get_inst_count() const {
    return this->m_cpu_state.get_inst_count();
}

uint32_t emulator_t::fetch() const {
    //Read a word from memory at the PC (using a "funct3" of 0b010 to get 32 bits)
    uint32_t inst = (uint32_t) this->m_memory.r(this->m_cpu_state.get_pc(), 0b010);

    //Log what we fetched and return it
    irvelog(1, "Fetched 0x%08x from 0x%08x", inst, this->m_cpu_state.get_pc());
    return inst;
}

/* Static Function Implementations */

//TODO
