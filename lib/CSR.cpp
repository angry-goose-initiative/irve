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
CSR::CSR_t::CSR_t() : medeleg(0), mideleg(0), minstret(0), m_privilege_mode(privilege_mode_t::MACHINE_MODE) {}

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
        //case address::SSCRATCH:         return this->sscratch;//TODO
        //case address::SEPC:             return this->sepc;//TODO
        //case address::SCAUSE:           return this->scause;//TODO
        //case address::STVAL:            return this->stval;//TODO
        //case address::SIP:              return this->sip;//TODO
        //case address::SATP:             return this->satp;//TODO figure out which satp is which
        //case address::MSTATUS:          return this->mstatus;//TODO
        case address::MISA:             return 0;
        case address::MEDELEG:          return this->medeleg;
        case address::MIDELEG:          return this->mideleg;
        //case address::MIE:              return this->mie;//TODO
        case address::MTVEC:            return MTVEC_CONTENTS;
        case address::MCOUNTEREN:       return 0;//Since we chose to make this 0, we don't need to implement any user-mode-facing counters
        //case address::MENVCFG:          return this->menvcfg & 0b1;
        //case address::MSTATUSH:         return this->mstatush;//TODO
        case address::MCOUNTINHIBIT:    return 0;

        case address::MHPMEVENT_START ... address::MHPMEVENT_END: return 0;

        case address::MSCRATCH:         return this->mscratch;
        case address::MEPC:             return this->mepc;
        case address::MCAUSE:           return this->mcause;
        case address::MTVAL:            return 0;
        //case address::MIP:              return this->mip;
        //case address::MTINST:           return this->mtinst;//TODO
        //TODO the PMP CSRs
        //case address::SATP:             return this->satp;//TODO figure out which satp is which
        case address::MCYCLE:           return (uint32_t)(this->mcycle & 0xFFFFFFFF);
        case address::MINSTRET:         return (uint32_t)(this->minstret & 0xFFFFFFFF);

        case address::MHPMCOUNTER_START ... address::MHPMCOUNTER_END: return 0;

        case address::MCYCLEH:          return (uint32_t)(this->mcycle >> 32);
        case address::MINSTRETH:        return (uint32_t)(this->minstret >> 32);

        case address::MHPMCOUNTERH_START ... address::MHPMCOUNTERH_END: return 0;

        case address::MVENDORID:        return 0;
        case address::MARCHID:          return 0; 
        case address::MIMPID:           return 0; 
        case address::MHARTID:          return 0;
        case address::MCONFIGPTR:       return 0;
        default:                        assert(false && "Attempt to implicitly read from an invalid CSR address"); return 0;//TODO invoke an exception instead of asserting
    }
}

void CSR::CSR_t::implicit_write(uint16_t csr, word_t data) {//This should assert the address is valid
    switch (csr) {
        //case address::SSCRATCH:         //TODO
        //case address::SEPC:             //TODO
        //case address::SCAUSE:           //TODO
        //case address::STVAL:            //TODO
        //case address::SIP:              //TODO
        //case address::SATP:             return this->satp;//TODO figure out which satp is which
        //case address::MSTATUS:          //TODO
        case address::MISA:             return;//We simply ignore writes to MISA, NOT throw an exception
        case address::MEDELEG:          this->medeleg = data; return;
        case address::MIDELEG:          this->mideleg = data; return;
        //case address::MIE:              //TODO
        //case address::MTVEC:            //TODO
        case address::MENVCFG:          this->menvcfg = data & 0b1; return;
        //case address::MSTATUSH:         //TODO
        case address::MCOUNTINHIBIT:    return;//We simply ignore writes to MCOUNTINHIBIT, NOT throw an exception
        //TODO the event counters
        case address::MSCRATCH:         this->mscratch = data; return;
        case address::MEPC:             this->mepc = data & 0xFFFFFFFC; return;
        case address::MCAUSE:           this->mcause = data; return;
        case address::MTVAL:              return;//We simply ignore writes to MTVAL, NOT throw an exception//TODO since we chose to make it read-only, should we throw an exception?
        //case address::MIP:              //TODO
        //case address::MTINST:           //TODO
        //case address::MTVAL2:           //TODO
        //TODO the PMP CSRs
        //case address::SATP:             return this->satp;//TODO figure out which satp is which
        //case address::MCYCLE:           //TODO 
        //case address::MINSTRET:         //TODO

        case address::MHPMCOUNTER_START ... address::MHPMCOUNTER_END: return;//We simply ignore writes to the HPMCOUNTER CSRs, NOT throw exceptions

        //case address::MCYCLEH:          //TODO
        //case address::MINSTRETH:        //TODO

        case address::MHPMCOUNTERH_START ... address::MHPMCOUNTERH_END: return;//We simply ignore writes to the HPMCOUNTERH CSRs, NOT throw exceptions

        default:                        assert(false && "Attempt to implicitly write to an invalid or read-only CSR address"); return;//TODO invoke an exception instead of asserting
    }
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
    //TODO use bits in the csr to determine if it can be read from at the current privilege mode
    /*if((csr & 0b1100000000) > ((uint16_t)(m_privilege_mode) << 8)) {//FIXME avoid comparing integers of different signedness
        throw rvexception_t(ILLEGAL_INSTRUCTION);
    }*/
    return true;
}

bool CSR::CSR_t::valid_explicit_write_at_current_privilege_mode(uint16_t csr) const {
    if (((csr >> 10) & 0b11) == 0b11) {//If top 2 bits are 1, then it's a read only CSR
        return false;
    }
    //TODO use bits in the csr to determine if it can be read from at the current privilege mode
    /*
    if((csr >> 10) == 0b11 || (csr & 0b1100000000) > ((uint16_t)(m_privilege_mode) << 8)) {//FIXME avoid comparing integers of different signedness
        throw rvexception_t(ILLEGAL_INSTRUCTION);
    }
    */
    return true;
}
