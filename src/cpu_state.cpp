/* NAME//TODO
 * By: John Jekel & Nick Chan
 *
 * TODO description
 *
*/

/* Constants And Defines */

//TODO

/* Includes */

#include "cpu_state.h"

#include <cassert>
#include <cstdint>

#define INST_COUNT this->get_inst_count()
#include "logging.h"

/* Types */

//TODO

/* Variables */

//TODO

/* Static Function Declarations */

//TODO

/* Function Implementations */

cpu_state_t::cpu_state_t(): m_inst_count(0), m_pc(0), m_regs() {
    irvelog(1, "Created new cpu_state instance");
    this->log(1);
}

void cpu_state_t::increment_inst_count() {
    ++this->m_inst_count;
}

uint64_t cpu_state_t::get_inst_count() const {
    return this->m_inst_count;
}

uint32_t cpu_state_t::get_pc() const {
    return this->m_pc;
}

void cpu_state_t::set_pc(uint32_t new_pc) {
    this->m_pc = new_pc;
}

Reg cpu_state_t::get_r(uint8_t reg_num) const {
    assert((reg_num >= 0) && (reg_num < 32) && "Attempted to access invalid register");
    return this->m_regs[reg_num];
}

void cpu_state_t::set_r(uint8_t reg_num, uint32_t new_val) {
    assert((reg_num >= 0) && (reg_num < 32) && "Attempted to access invalid register");
    if (reg_num != 0) {
        this->m_regs[reg_num].u = new_val;
    }
}

void cpu_state_t::set_r(uint8_t reg_num, int32_t new_val) {
    assert((reg_num >= 0) && (reg_num < 32) && "Attempted to access invalid register");
    if (reg_num != 0) {
        this->m_regs[reg_num].s = new_val;
    }
}

void cpu_state_t::log(uint8_t indent) const {
    irvelog(indent + 1, "Inst Count: %lu", this->get_inst_count());
    irvelog(indent + 1, "PC:\t\t0x%08x", this->get_pc());
    irvelog(indent + 1, "Registers:");
    for (uint8_t i = 0; i < 32; ++i) {
        irvelog(indent + 2, "x%u:\t0x%08x", i, this->get_r(i).u);
    }
}

/* Static Function Implementations */

//TODO
