/**
 * @brief   Holds a RISC-V hart's state including registers and the PC
 * 
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  See the LICENSE file at the root of the project for licensing info.
*/

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include "cpu_state.h"

#include <cassert>
#include <cstdint>

#define INST_COUNT 0
#include "logging.h"

#include "rv_trap.h"

#include "fuzzish.h"

using namespace irve::internal;

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

CpuState::CpuState() :
    m_pc(0),
    m_atomic_reservation_set_valid(false)//At reset, no LR has been executed yet
{
    irvelog(1, "Created new CpuState instance");

    //Initialize all registers to a random number (just to prevent any sanitizers from complaining)
    for (uint8_t i = 0; i < 31; ++i) {
        this->m_regs[i] = irve_fuzzish_rand();
    }

    this->log(2);
}

Word CpuState::get_pc() const {
    return this->m_pc;
}

void CpuState::set_pc(Word new_pc) {
    this->m_pc = new_pc;
}

Reg CpuState::get_r(uint8_t reg_num) const {
    assert(reg_num < 32 && "Attempt to get invalid register");
    if (reg_num) {
        return this->m_regs[reg_num - 1];
    } else {
        return 0;
    }
}

void CpuState::set_r(uint8_t reg_num, Reg new_val) {
    assert(reg_num < 32 && "Attempt to set invalid register");
    if (reg_num) {
        this->m_regs[reg_num - 1] = new_val;
    }
}

void CpuState::log([[maybe_unused]] uint8_t indent) const {
    //irvelog(indent, "Inst Count: %lu", this->get_inst_count());
    irvelog(indent, "PC:\t\t0x%08x", this->get_pc());
    irvelog(indent, "Registers:");
    for (uint8_t i = 0; i < 32; ++i) {
        irvelog(indent + 1, "x%u:\t0x%08x", i, this->get_r(i).u);
    }

    irvelog(indent, "TODO print CSRs here");

    /*
    switch (this->m_privilege_mode) {
        case PrivilegeMode::USER_MODE:
            irvelog(indent, "Privilege Mode: User");
            break;
        case PrivilegeMode::SUPERVISOR_MODE:
            irvelog(indent, "Privilege Mode: Supervisor");
            break;
        case PrivilegeMode::MACHINE_MODE:
            irvelog(indent, "Privilege Mode: Machine");
            break;
        default:
            assert(false && "Invalid privilege mode");
            break;
    }
    */
}

void CpuState::validate_reservation_set() {
    this->m_atomic_reservation_set_valid = true;
}

void CpuState::invalidate_reservation_set() {
    this->m_atomic_reservation_set_valid = false;
}

bool CpuState::reservation_set_valid() const {
    return this->m_atomic_reservation_set_valid;
}

void CpuState::goto_next_sequential_pc() {
    this->m_pc += 4;
    irvelog(3, "Going to next sequential PC: 0x%08X", this->m_pc);
}
