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
CSR::CSR_t::CSR_t() : medeleg(0), mideleg(0), m_privilege_mode(privilege_mode_t::MACHINE_MODE), minstret(0) {
}

reg_t CSR::CSR_t::explicit_read(uint16_t csr) const {//Should throw exceptions if the address is invalid
    //TODO address checking and ILLEGAL_INSTRUCTION_EXCEPTION throwing
    return this->implicit_read(csr);
}

void CSR::CSR_t::explicit_write(uint16_t csr, word_t data) {//Should throw exceptions if the address is invalid
    //TODO address checking and ILLEGAL_INSTRUCTION_EXCEPTION throwing
    this->implicit_write(csr, data);
}

reg_t CSR::CSR_t::implicit_read(uint16_t csr) const {//This should assert the address is valid
    //TODO do this properly
    // TODO check if CSR can be read from
    if (csr == 0x341) {
        return this->mepc;
    }
    if (csr == 0x342) {
        return this->mcause.as_reg_t;
    }
   /* if((csr & 0b1100000000) > ((uint16_t)(m_privilege_mode) << 8)) {//FIXME avoid comparing integers of different signedness
        throw rvexception_t(ILLEGAL_INSTRUCTION);
    }
    */
    assert(false && "TODO");
}

void CSR::CSR_t::implicit_write(uint16_t csr, word_t data) {//This should assert the address is valid
    //TODO do this properly
    // TODO check if CSR can be written to
    if (csr == 0x341) {
        this->mepc = data & 0xFFFFFFFC;
        return;
    }
    if (csr == 0x342) {
        this->mcause.as_reg_t = data;
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

void CSR::CSR_t::set_privilege_mode(privilege_mode_t new_privilege_mode) {
    //TODO sanity check this is a valid mode
    this->m_privilege_mode = new_privilege_mode;
}

CSR::privilege_mode_t CSR::CSR_t::get_privilege_mode() const {
    return this->m_privilege_mode;
}

void CSR::CSR_t::increment_inst_count() {
    ++this->minstret;
}

uint64_t CSR::CSR_t::get_inst_count() const {
    return this->minstret;
}
