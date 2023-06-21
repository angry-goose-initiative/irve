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

    /**
     * @brief Holds a RISC-V hart's "CPU state" including registers, the PC, the current load reservation, but not CSRs
    */
    class cpu_state_t {
    public:
        /**
         * @brief Construct a new cpu_state_t
         * @param CSR_ref A reference to the CSR object that this cpu_state_t object will use internally
         *
        */
        cpu_state_t(irve::internal::CSR::CSR_t& CSR_ref);

        /**
         * @brief Get the current value of the PC
         * @return The PC
        */
        reg_t get_pc() const;

        /**
         * @brief Set the PC to a new value
         * @param new_pc The new value of the PC
        */
        void set_pc(reg_t new_pc);

        /**
         * @brief Get the current value of a register
         * @param reg_num The register number to get the value of (between 0 and 31 inclusive)
         * @return The value of the register
        */
        reg_t get_r(uint8_t reg_num) const;

        /**
         * @brief Set the value of a register
         * @param reg_num The register number to set the value of (between 0 and 31 inclusive)
         * @param new_val The new value of the register
        */
        void set_r(uint8_t reg_num, reg_t new_val);

        void log(uint8_t indent) const;

        /**
         * @brief Set a load reservation (LR should call this)
        */
        void validate_reservation_set();

        /**
         * @brief Invalidate a load reservation (SC should call this; it should also be called on unexpected control flow changes)
        */
        void invalidate_reservation_set();

        /**
         * @brief Check if the load reservation is valid
         * @return True if the load reservation is valid, false otherwise
        */
        bool reservation_set_valid() const;

        /**
         * @brief Increment the current PC by 4
        */
        void goto_next_sequential_pc();
    private:
        reg_t m_pc;
        reg_file::reg_file_t m_regs;
        CSR::CSR_t& m_CSR_ref;
        bool m_atomic_reservation_set_valid;
    };
}

#endif//CPU_STATE_H
