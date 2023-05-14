/* CSR.cpp
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * TODO description
 *
*/

/* Includes */

#include "CSR.h"

#include <cassert>
#include <cstddef>

#include "rvexception.h"

using namespace irve::internal;

/* Function Implementations */

// TODO what should CSRs be initialized to?
CSR::CSR_t::CSR_t() {
    for (std::size_t i = 0; i < 32; ++i) {
        this->medeleg[i] = false;
        this->mideleg[i] = false;
    }
}

reg_t CSR::CSR_t::get(uint16_t csr) const {
    //TODO do this properly
    // TODO check if CSR can be read from
    if (csr == 0x341) {
        return this->mepc;
    }
    if (csr == 0x342) {
        return (uint32_t)this->mcause;
    }
   /* if((csr & 0b1100000000) > ((uint16_t)(m_privilege_mode) << 8)) {//FIXME avoid comparing integers of different signedness
        throw rvexception_t(ILLEGAL_INSTRUCTION);
    }
    */
    assert(false && "TODO");
}

void CSR::CSR_t::set(uint16_t csr, word_t data) {
    //TODO do this properly
    // TODO check if CSR can be written to
    if (csr == 0x341) {
        this->mepc = data & 0xFFFFFFFC;
        return;
    }
    if (csr == 0x342) {
        this->mcause = (cause_t)data.u;
        return;
    }
    /*
    if((csr >> 10) == 0b11 || (csr & 0b1100000000) > ((uint16_t)(m_privilege_mode) << 8)) {//FIXME avoid comparing integers of different signedness
        throw rvexception_t(ILLEGAL_INSTRUCTION);
    }
    */

    //TODO some CSRs are read only, some are write only, some are read/write
    //Sometimes only PARTS of a CSR are writable or affect other bits
    //We need to check for that and deal with it here
    assert(false && "TODO");
}
