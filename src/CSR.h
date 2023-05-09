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

//TODO fill in the x's with supported extensions
#define MISA 0b010000xxxxxxxxxxxxxxxxxxxxxxxxxx

//TODO we can hardcode MTVEC, so we should, but to where?
#define MTVEC 0xE0000000

/* Function/Class Declarations */

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


    //TODO add CSRs HERE
};

#endif
