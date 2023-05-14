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

/* Types */

enum class privilege_mode_t : uint8_t {//TODO move this to execute
    USER_MODE = 0b00,
    SUPERVISOR_MODE = 0b01,
    MACHINE_MODE = 0b11
};

/* Function/Class Declarations */

namespace irve::internal::CSR {
    class CSR_t {
    public:
        CSR_t();

        irve::internal::reg_t get(uint16_t csr) const;
        void set(uint16_t csr, irve::internal::word_t data);

        void set_privilege_mode(privilege_mode_t new_privilege_mode);//TODO move this to execute
        privilege_mode_t get_privilege_mode() const;//TODO move this to execute


        //TODO make these private

        reg_t sscratch;//Address 0x140
        reg_t sepc;//Address 0x141
        cause_t scause;//Address 0x142
        reg_t stval;//Address 0x143
        //TODO sip here or somewhere else?//Address 0x144
        //TODO satp here or somewhere else?//Address 0x180

        //TODO mstatus here or somewhere else?//Address 0x300
        //misa is NOT here//Address 0x301
        bool medeleg[32];//Address 0x302
        bool mideleg[32];//Address 0x303
        //TODO mie here or somewhere else?//Address 0x304
        //TODO mtvec here or somewhere else?//Address 0x305
        //TODO mcounteren here or somewhere else?//Address 0x306
        //TODO menvcfg here or somewhere else?//Address 0x30A
        //TODO mstatush here or somewhere else?//Address 0x310

        reg_t mepc;//Address 0x341
        cause_t mcause;//Address 0x342

        //TODO add CSRs HERE

    private:
        privilege_mode_t m_privilege_mode;//Not a CSR, but it is a register we need to access to determine if we can access a CSR (and it is also used in other places)
    };
}

#endif
