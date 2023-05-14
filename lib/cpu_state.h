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

/* Function/Class Declarations */

namespace irve::internal::cpu_state {

    class cpu_state_t {
    public:
        cpu_state_t(irve::internal::CSR::CSR_t& CSR_ref);

        reg_t get_pc() const;
        void set_pc(reg_t new_pc);

        reg_t get_r(uint8_t reg_num) const;
        void set_r(uint8_t reg_num, reg_t new_val);

        void log(uint8_t indent) const;

        void validate_reservation_set();
        void invalidate_reservation_set();
        bool reservation_set_valid() const;

        void goto_next_sequential_pc();
    private:
        reg_t m_pc;
        reg_file::reg_file_t m_regs;
        CSR::CSR_t& m_CSR_ref;
        bool m_atomic_reservation_set_valid;
    };

}

#endif//CPU_STATE_H
