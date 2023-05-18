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
    if (!this->valid_explicit_read_at_current_privilege_mode(csr)) {
        invoke_rv_exception(ILLEGAL_INSTRUCTION);
    } else {
        return this->implicit_read(csr);
    }
}

void CSR::CSR_t::explicit_write(uint16_t csr, word_t data) {//Should throw exceptions if the address is invalid
    if (!this->valid_explicit_write_at_current_privilege_mode(csr)) {
        invoke_rv_exception(ILLEGAL_INSTRUCTION);
    } else {
        this->implicit_write(csr, data);
    }
}

reg_t CSR::CSR_t::implicit_read(uint16_t csr) const {//This should assert the address is valid
    switch (csr) {
        case address::SSCRATCH:         return this->sscratch;
        case address::SEPC:             return this->sepc;
        case address::SCAUSE:           return this->scause;
        case address::STVAL:            return this->stval;
        case address::SIP:              return this->sip;
        //case address::SATP:             return this->satp;//TODO figure out which satp is which
        case address::MSTATUS:          return this->mstatus;
        case address::MISA:             return MISA_CONTENTS;
        case address::MEDELEG:          return this->medeleg;
        case address::MIDELEG:          return this->mideleg;
        case address::MIE:              return this->mie;
        case address::MTVEC:            return this->mtvec;
        case address::MCOUNTEREN:       return this->mcounteren;
        case address::MENVCFG:          return this->menvcfg;
        case address::MSTATUSH:         return this->mstatush;
        case address::MCOUNTINHIBIT:    return this->mcountinhibit;
        //TODO the event counters
        case address::MSCRATCH:         return this->mscratch;
        case address::MEPC:             return this->mepc;
        case address::MCAUSE:           return this->mcause;
        case address::MTVAL:            return this->mtval;
        case address::MIP:              return this->mip;
        case address::MTINST:           return this->mtinst;
        case address::MTVAL2:           return this->mtval2;
        //TODO the PMP CSRs
        //case address::SATP:             return this->satp;//TODO figure out which satp is which
        case address::MCYCLE:           return this->mcycle;
        case address::MINSTRET:         return (uint32_t)(this->minstret & 0xFFFFFFFF);
        //TODO the event counters
        case address::MCYCLEH:          return this->mcycleh;
        case address::MINSTRETH:        return (uint32_t)(this->minstret >> 32);
        //TODO the event counters
        case address::CYCLE:            return this->cycle;
        case address::TIME:             return this->time;
        case address::INSTRET:          return this->instret;
        //TODO the event counters
        case address::CYCLEH:           return this->cycleh;
        case address::TIMEH:            return this->timeh;
        case address::INSTRETH:         return this->instreth;
        //TODO the event counters
        case address::MVENDORID:        return 0;
        case address::MARCHID:          return 0; 
        case address::MIMPID:           return 0; 
        case address::MHARTID:          return 0;
        case address::MCONFIGPTR:       return 0;
        default:                        assert(false && "Attempt to implicitly read from invalid CSR address"); return 0;
    }
    //TODO this is just a switch statement, with a default case that asserts false
    //TODO do this properly
    // TODO check if CSR can be read from
    /*
    if (csr == 0x341) {
        return this->mepc;
    }
    if (csr == 0x342) {
        return this->mcause.as_reg_t;
    }
    if((csr & 0b1100000000) > ((uint16_t)(m_privilege_mode) << 8)) {//FIXME avoid comparing integers of different signedness
        throw rvexception_t(ILLEGAL_INSTRUCTION);
    }
    
    assert(false && "TODO");
    */
}

void CSR::CSR_t::implicit_write(uint16_t csr, word_t data) {//This should assert the address is valid
    //TODO this is just a switch statement, with a default case that asserts false
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

bool CSR::CSR_t::valid_explicit_read_at_current_privilege_mode(uint16_t /* csr */) const {
    //TODO
    return true;
}

bool CSR::CSR_t::valid_explicit_write_at_current_privilege_mode(uint16_t /* csr */) const {
    //TODO
    return true;
}
