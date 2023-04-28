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

/* Types */

//TODO

/* Variables */

//TODO

/* Static Function Declarations */

//TODO

/* Function Implementations */

cpu_state_t::cpu_state_t(): m_inst_count(0), m_pc(0), m_regs() {}

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

/* Static Function Implementations */

//TODO
