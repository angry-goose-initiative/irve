/* cpu_state.cpp
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Holds a RISC-V hart's state including registers, PC, and CSRs
 *
*/

/* Constants And Defines */

/* Includes */

#include "cpu_state.h"

#include <cassert>
#include <cstdint>

#define INST_COUNT this->m_CSR_ref.get_inst_count()
#include "logging.h"

#include "rvexception.h"

using namespace irve::internal;

/* Function Implementations */

cpu_state_t::cpu_state_t(CSR::CSR_t& CSR_ref) :
    m_pc(0),
    m_regs(),
    m_CSR_ref(CSR_ref),
    m_atomic_reservation_set_valid(false)//At reset, no LR has been executed yet
{
    irvelog(1, "Created new cpu_state instance");
    this->log(2);
}

word_t cpu_state_t::get_pc() const {
    return this->m_pc;
}

void cpu_state_t::set_pc(word_t new_pc) {
    this->m_pc = new_pc;
}

reg_t cpu_state_t::get_r(uint8_t reg_num) const {
    return this->m_regs[reg_num];
}

void cpu_state_t::set_r(uint8_t reg_num, reg_t new_val) {
    this->m_regs[reg_num] = new_val;
}

void cpu_state_t::log(uint8_t indent) const {
    //irvelog(indent, "Inst Count: %lu", this->get_inst_count());
    irvelog(indent, "PC:\t\t0x%08x", this->get_pc());
    irvelog(indent, "Registers:");
    for (uint8_t i = 0; i < 32; ++i) {
        irvelog(indent + 1, "x%u:\t0x%08x", i, this->get_r(i).u);
    }

    irvelog(indent, "TODO print CSRs here");

    /*
    switch (this->m_privilege_mode) {
        case privilege_mode_t::USER_MODE:
            irvelog(indent, "Privilege Mode: User");
            break;
        case privilege_mode_t::SUPERVISOR_MODE:
            irvelog(indent, "Privilege Mode: Supervisor");
            break;
        case privilege_mode_t::MACHINE_MODE:
            irvelog(indent, "Privilege Mode: Machine");
            break;
        default:
            assert(false && "Invalid privilege mode");
            break;
    }
    */
}

void cpu_state_t::validate_reservation_set() {
    this->m_atomic_reservation_set_valid = true;
}

void cpu_state_t::invalidate_reservation_set() {
    this->m_atomic_reservation_set_valid = false;
}

bool cpu_state_t::reservation_set_valid() const {
    return this->m_atomic_reservation_set_valid;
}

void cpu_state_t::goto_next_sequential_pc() {
    this->m_pc += 4;
    irvelog(3, "Going to next sequential PC: 0x%08X", this->m_pc);
}
