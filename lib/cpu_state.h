/* cpu_state.h
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Holds a RISC-V hart's state including registers, PC, and CSRs
 *
*/

#ifndef CPU_STATE_H
#define CPU_STATE_H

/* Includes */

#include <cstdint>

#include "reg_file.h"
#include "CSR.h"
#include "memory.h"

#include "rvexception.h"

/* Types */

//TODO namespacing

enum class privilege_mode_t : uint8_t {//TODO move this to execute
    USER_MODE = 0b00,
    SUPERVISOR_MODE = 0b01,
    MACHINE_MODE = 0b11
};

/* Function/Class Declarations */

//TODO namespacing

class cpu_state_t {
public:
    cpu_state_t(irve::internal::CSR::CSR_t& CSR_ref);

    //irve::internal::reg_t get_CSR(uint16_t csr) const;//TODO move to CSR
    //void set_CSR(uint16_t csr, irve::internal::word_t data);//TODO move to CSR

    void increment_inst_count();
    uint64_t get_inst_count() const;

    irve::internal::reg_t get_pc() const;
    void set_pc(irve::internal::reg_t new_pc);

    irve::internal::reg_t get_r(uint8_t reg_num) const;
    void set_r(uint8_t reg_num, irve::internal::reg_t new_val);

    void log(uint8_t indent) const;

    void set_privilege_mode(privilege_mode_t new_privilege_mode);//TODO move this to execute
    privilege_mode_t get_privilege_mode() const;//TODO move this to execute

    void handle_interrupt(cause_t cause);//TODO move to either emulator or execute
    void handle_exception(cause_t cause);//TODO move to either emulator or execute

    void validate_reservation_set();
    void invalidate_reservation_set();
    bool reservation_set_valid() const;

    void goto_next_sequential_pc();
private:
    privilege_mode_t m_privilege_mode;//TODO Moving to emulator
    uint64_t m_inst_count;//TODO Moving to CSR
    irve::internal::reg_t m_pc;//Staying here
    reg_file_t m_regs;//Staying here
    irve::internal::CSR::CSR_t& m_CSR_ref;
    bool m_atomic_reservation_set_valid;//NOT moving to emulator; emulator will call invalidate_reservation_set() when an exception occurs

    //TODO interrupts
};

#endif//CPU_STATE_H
