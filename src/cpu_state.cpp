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

#define INST_COUNT this->get_inst_count()
#include "logging.h"

/* Function Implementations */

cpu_state_t::cpu_state_t(memory_t& memory_ref) :
    m_privilege_mode(MACHINE_MODE),
    m_CSR(),
    m_inst_count(0), 
    m_pc(0),
    m_regs(),
    m_memory_ref(memory_ref)
{
    irvelog(1, "Created new cpu_state instance");
    this->log(2);
}

reg_t cpu_state_t::get_CSR(reg_t csr) const {
    // TODO check if CSR can be read from
    if((csr.u & 0b1100000000) > ((uint16_t)(m_privilege_mode) << 8)) {
        // if not readable, throw exception to be caught?
    }
    return reg_t();
}

void cpu_state_t::set_CSR(reg_t csr, reg_t data) {
    // TODO check if CSR can be written to
    if((csr.u >> 10) == 0b11 || (csr.u & 0b1100000000) > ((uint16_t)(m_privilege_mode) << 8)) {
        // if not writeable, throw exception to be caught?
    }
    //TODO some CSRs are read only, some are write only, some are read/write
    //Sometimes only PARTS of a CSR are writable or affect other bits
    //We need to check for that and deal with it here
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

reg_t cpu_state_t::get_r(uint8_t reg_num) const {
    return this->m_regs[reg_num];
}

void cpu_state_t::set_r(uint8_t reg_num, reg_t new_val) {
    this->m_regs[reg_num] = new_val;
}

void cpu_state_t::set_r(uint8_t reg_num, uint32_t new_val) {
    this->m_regs[reg_num].u = new_val;
}

void cpu_state_t::set_r(uint8_t reg_num, int32_t new_val) {
    this->m_regs[reg_num].s = new_val;
}

void cpu_state_t::log(uint8_t indent) const {

    irvelog(indent, "Inst Count: %lu", this->get_inst_count());
    irvelog(indent, "PC:\t\t0x%08x", this->get_pc());
    irvelog(indent, "Registers:");
    for (uint8_t i = 0; i < 32; ++i) {
        irvelog(indent + 1, "x%u:\t0x%08x", i, this->get_r(i).u);
    }

    irvelog(indent, "TODO print CSRs here");

    switch (this->m_privilege_mode) {
        case USER_MODE:
            irvelog(indent, "Privilege Mode: User");
            break;
        case SUPERVISOR_MODE:
            irvelog(indent, "Privilege Mode: Supervisor");
            break;
        case MACHINE_MODE:
            irvelog(indent, "Privilege Mode: Machine");
            break;
        default:
            assert(false && "Invalid privilege mode");
            break;
    }
}
