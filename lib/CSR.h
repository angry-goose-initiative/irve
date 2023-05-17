/* CSR.h
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * TODO description
 *
*/

#ifndef CSR_H
#define CSR_H

/* Includes */

#include <cstdint>

#include "common.h"
#include "rvexception.h"

/* Constants and Defines */

#define MEPC_ADDRESS 0x341

//TODO fill in the x's with supported extensions
#define MISA word_t(0b010000xxxxxxxxxxxxxxxxxxxxxxxxxx)

//MTVEC hardcoded to start at 0x00000004 and be vectored
#define MTVEC word_t((0x00000004 << 2) | 0b01)

namespace irve::internal::CSR {

    /* Types */

    enum class privilege_mode_t : uint8_t {
        USER_MODE = 0b00,
        SUPERVISOR_MODE = 0b01,
        MACHINE_MODE = 0b11
    };

    /* Function/Class Declarations */

    class CSR_t {
    public:
        CSR_t();

        reg_t get(uint16_t csr) const;
        void set(uint16_t csr, word_t data);

        void set_privilege_mode(privilege_mode_t new_privilege_mode);
        privilege_mode_t get_privilege_mode() const;

        //TODO add way to implicitly read/write CSRs so they won't cause exceptions (ex. for timers, etc.)

        void increment_inst_count();
        uint64_t get_inst_count() const;


        //TODO make these private

        reg_t sscratch;//Address 0x140
        reg_t sepc;//Address 0x141
        union {//Address 0x142
            rvexception::cause_t as_cause_t;
            reg_t as_reg_t;
        } scause;
        reg_t stval;//Address 0x143
        //TODO sip here or somewhere else?//Address 0x144
        //TODO satp here or somewhere else?//Address 0x180

        //TODO mstatus here or somewhere else?//Address 0x300
        //misa is NOT here//Address 0x301
        reg_t medeleg;//Address 0x302
        reg_t mideleg;//Address 0x303
        //TODO mie here or somewhere else?//Address 0x304
        //TODO mtvec here or somewhere else?//Address 0x305
        //TODO mcounteren here or somewhere else?//Address 0x306
        //TODO menvcfg here or somewhere else?//Address 0x30A
        //TODO mstatush here or somewhere else?//Address 0x310

        reg_t mepc;//Address 0x341
        union {
            rvexception::cause_t as_cause_t;//Address 0x342
            reg_t as_reg_t;
        } mcause;

        //TODO add CSRs HERE

    private:
        privilege_mode_t m_privilege_mode;//Not a CSR, but it is a register we need to access to determine if we can access a CSR (and it is also used in other places)
        uint64_t minstret;
    };
}

#endif
