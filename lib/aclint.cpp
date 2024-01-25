/**
 * @brief   Implementation of RISC-V ACLINT
 * 
 * @copyright
 *  Copyright (C) 2024 John Jekel\n
 *  See the LICENSE file at the root of the project for licensing info.
 * 
 * Bare-bones, only implementing the timer portion and only one mtimecmp register.
 *
*/

#include <assert.h>
#include <stdint.h>

#include "aclint.h"

#include "common.h"
#include "csr.h"
#define INST_COUNT 0
#include "logging.h"

using namespace irve::internal;

Aclint::Aclint(Csr& csrs) : m_csrs(csrs) {}

Word Aclint::read(Aclint::Address register_address) {
    assert(((static_cast<uint16_t>(register_address) & 0b11) == 0) && "Unaligned access to ACLINT");

    irvelog(10, "ACLINT read from offset 0x%04X\n", register_address);

    //TODO other ACLINT features if they are needed by the kernel

    Csr::Address csr_addr;
    switch (register_address) {
        case Address::MTIME:        csr_addr = Csr::Address::MTIME; break;
        case Address::MTIMEH:       csr_addr = Csr::Address::MTIMEH; break;
        case Address::MTIMECMP:     csr_addr = Csr::Address::MTIMECMP; break;
        case Address::MTIMECMPH:    csr_addr = Csr::Address::MTIMECMPH; break;
        default: return 0;//All other mtimecmp registers are read as 0 (also the MSWI isn't implemented)
    }

    return this->m_csrs.implicit_read(csr_addr);
}

void Aclint::write(Aclint::Address register_address, Word data) {
    assert(((static_cast<uint16_t>(register_address) & 0b11) == 0) && "Unaligned access to ACLINT");

    irvelog(10, "ACLINT write to offset 0x%04X with data 0x%08X\n", register_address, data.u);

    //TODO other ACLINT features if they are needed by the kernel

    Csr::Address csr_addr;
    switch (register_address) {
        case Address::MTIME:        csr_addr = Csr::Address::MTIME; break;
        case Address::MTIMEH:       csr_addr = Csr::Address::MTIMEH; break;
        case Address::MTIMECMP:     csr_addr = Csr::Address::MTIMECMP; break;
        case Address::MTIMECMPH:    csr_addr = Csr::Address::MTIMECMPH; break;
        default: return;//Ignore writes to other timecmp registers (also the MSWI isn't implemented)
    }

    this->m_csrs.implicit_write(csr_addr, data);
}
