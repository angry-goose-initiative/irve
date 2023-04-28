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

cpu_state_t::cpu_state_t(): insts_retired(0), pc(0), reg() {}

void cpu_state_t::retire_inst() {
    ++insts_retired;
}

uint64_t cpu_state_t::get_insts_retired() const {
    return insts_retired;
}

uint32_t cpu_state_t::get_pc() const {
    return pc;
}

void cpu_state_t::set_pc(uint32_t new_pc) {
    pc = new_pc;
}

Reg cpu_state_t::get_r(uint8_t reg_num) const {
    assert((reg_num >= 0) && (reg_num < 32) && "Attempted to access invalid register");
    return reg[reg_num];
}

void cpu_state_t::set_r(uint8_t reg_num, uint32_t new_val) {
    assert((reg_num >= 0) && (reg_num < 32) && "Attempted to access invalid register");
    if (reg_num != 0) {
        reg[reg_num].u = new_val;
    }
}

void cpu_state_t::set_r(uint8_t reg_num, int32_t new_val) {
    assert((reg_num >= 0) && (reg_num < 32) && "Attempted to access invalid register");
    if (reg_num != 0) {
        reg[reg_num].s = new_val;
    }
}

/* Static Function Implementations */

//TODO
