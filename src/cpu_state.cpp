/* NAME//TODO
 * By: John Jekel
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

uint32_t cpu_state_t::get_r(uint8_t reg_num) const {
    assert((reg_num >= 0) && (reg_num < 31) && "Attempted to access invalid register");
    if (reg_num == 0) {
        return 0;
    } else {
        return reg[reg_num - 1];
    }
}

void cpu_state_t::set_r(uint8_t reg_num, uint32_t new_val) {
    assert((reg_num >= 0) && (reg_num < 31) && "Attempted to access invalid register");
    if (reg_num != 0) {
        reg[reg_num - 1] = new_val;
    }
}

/* Static Function Implementations */

//TODO
