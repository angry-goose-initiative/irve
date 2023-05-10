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

enum class privilege_mode_t : uint8_t {
    USER_MODE = 0b00,
    SUPERVISOR_MODE = 0b01,
    MACHINE_MODE = 0b11
};

/* Function/Class Declarations */

class cpu_state_t {
public:
    //We need a reference to memory so we can access the page table and virtual memory settings of the class
    cpu_state_t(memory_t& memory_ref);

    reg_t get_CSR(uint16_t csr) const;
    void set_CSR(uint16_t csr, word_t data);

    void increment_inst_count();
    uint64_t get_inst_count() const;

    reg_t get_pc() const;
    void set_pc(reg_t new_pc);

    reg_t get_r(uint8_t reg_num) const;
    void set_r(uint8_t reg_num, reg_t new_val);

    void log(uint8_t indent) const;

    void set_privilege_mode(privilege_mode_t new_privilege_mode);
    privilege_mode_t get_privilege_mode() const;

    void handle_interrupt(cause_t cause);
    void handle_exception(cause_t cause);
private:
    CSR_t m_CSR;
    uint64_t m_inst_count;
    reg_t m_pc;
    reg_file_t m_regs;
    privilege_mode_t m_privilege_mode;
    memory_t& m_memory_ref;//Used for managing if virtual memory is enabled or not, the page table location, etc

    //TODO interrupts
};

#endif//CPU_STATE_H
