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

/* Types */

typedef enum {
    USER_MODE = 0b00,
    SUPERVISOR_MODE = 0b01,
    MACHINE_MODE = 0b11
} privilege_mode_t;

/* Function/Class Declarations */

class cpu_state_t {
public:
    //We need a reference to memory so we can access the page table and virtual memory settings of the class
    cpu_state_t(memory_t& memory_ref);

    reg_t get_CSR(reg_t csr) const;
    void set_CSR(reg_t csr, reg_t data);

    void increment_inst_count();
    uint64_t get_inst_count() const;

    uint32_t get_pc() const;
    void set_pc(uint32_t new_pc);

    reg_t get_r(uint8_t reg_num) const;
    void set_r(uint8_t reg_num, reg_t new_val);
    void set_r(uint8_t reg_num, uint32_t new_val);
    void set_r(uint8_t reg_num, int32_t new_val);

    privilege_mode_t m_privilege_mode;//TODO make this private?

    void log(uint8_t indent) const;

    //TODO stuff for setting interrupts
private:
    CSR_t m_CSR;
    uint64_t m_inst_count;
    uint32_t m_pc;
    reg_file_t m_regs;
    memory_t& m_memory_ref;//Used for managing if virtual memory is enabled or not, the page table location, etc

    //TODO interrupts
};

#endif//CPU_STATE_H
