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

using namespace irve::internal;

/* Constants and Defines */

#define MEPC_ADDRESS 0x341

//TODO fill in the x's with supported extensions
#define MISA word_t(0b010000xxxxxxxxxxxxxxxxxxxxxxxxxx)

//MTVEC hardcoded to start at 0x00000004 and be vectored
#define MTVEC word_t((0x00000004 << 2) | 0b01)

/* Function/Class Declarations */

namespace irve::internal::CSR {
    class CSR_t {
    public:
        CSR_t();

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
    };
}

#endif
