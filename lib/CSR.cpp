/* CSR.cpp
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Contains (mostly) CSRs for IRVE as well as methods to read and write them.
 *
*/

/* Includes */

#include "CSR.h"

#include <cassert>
#include <cstddef>

#include "rvexception.h"

using namespace irve::internal;

/* Function Implementations */

//TODO what should CSRs be initialized to?
//See Volume 2 Section 3.4
//FIXME also initialize ones that don't require it so that the assumptions of the implicit_read function is satasfied
CSR::CSR_t::CSR_t() : mstatus(0), minstret(0), m_privilege_mode(CSR::privilege_mode_t::MACHINE_MODE) {}

reg_t CSR::CSR_t::explicit_read(uint16_t csr) const {//Performs privilege checks
    if (!this->current_privilege_mode_can_explicitly_read(csr)) {
        invoke_rv_exception(ILLEGAL_INSTRUCTION);
    } else {
        return this->implicit_read(csr);
    }
}

void CSR::CSR_t::explicit_write(uint16_t csr, word_t data) {//Performs privilege checks
    if (!this->current_privilege_mode_can_explicitly_write(csr)) {
        invoke_rv_exception(ILLEGAL_INSTRUCTION);
    } else {
        this->implicit_write(csr, data);
    }
}

//We assume the CSRs within the class are "safe" for the purposes of reads
reg_t CSR::CSR_t::implicit_read(uint16_t csr) const {//Does not perform any privilege checks
    //TODO workaround MSVC not supporting non-standard case ranges
    switch (csr) {
        case address::SSTATUS:          return this->mstatus;//FIXME only some bits of mstatus are readable from sstatus
        case address::SIE:              return this->sie;
        case address::STVEC:            return this->stvec;
        case address::SCOUNTEREN:       return this->scounteren;
        case address::SENVCFG:          return this->senvcfg;
        case address::SSCRATCH:         return this->sscratch;
        case address::SEPC:             return this->sepc;
        case address::SCAUSE:           return this->scause;
        case address::STVAL:            return 0;
        case address::SIP:              return this->sip;
        case address::SATP:             return this->satp;
        case address::MSTATUS:          return this->mstatus;
        case address::MISA:             return 0;
        case address::MEDELEG:          return this->medeleg;
        case address::MIDELEG:          return this->mideleg;
        case address::MIE:              return this->mie;
        case address::MTVEC:            return MTVEC_CONTENTS;
        case address::MCOUNTEREN:       return 0;//Since we chose to make this 0, we don't need to implement any user-mode-facing counters
        case address::MENVCFG:          return this->menvcfg & 0b1;//Only lowest bit is RW
        case address::MSTATUSH:         return this->mstatush;
        case address::MENVCFGH:         return 0;
        case address::MCOUNTINHIBIT:    return 0;

#ifndef _MSC_VER//Not on MSVC
        case address::MHPMEVENT_START ... address::MHPMEVENT_END: return 0;
#endif

        case address::MSCRATCH:         return this->mscratch;
        case address::MEPC:             return this->mepc;
        case address::MCAUSE:           return this->mcause;
        case address::MTVAL:            return 0;
        case address::MIP:              return this->mip;

#ifndef _MSC_VER//Not on MSVC
        case address::PMPCFG_START  ... address::PMPCFG_END:    return this->pmpcfg [csr - address::PMPCFG_START];
        case address::PMPADDR_START ... address::PMPADDR_END:   return this->pmpaddr[csr - address::PMPADDR_START];
#endif

        case address::MCYCLE:           return (uint32_t)(this->mcycle & 0xFFFFFFFF);
        case address::MINSTRET:         return (uint32_t)(this->minstret & 0xFFFFFFFF);

#ifndef _MSC_VER//Not on MSVC
        case address::MHPMCOUNTER_START ... address::MHPMCOUNTER_END: return 0;
#endif

        case address::MCYCLEH:          return (uint32_t)((this->mcycle >> 32) & 0xFFFFFFFF);
        case address::MINSTRETH:        return (uint32_t)((this->minstret >> 32) & 0xFFFFFFFF);

#ifndef _MSC_VER//Not on MSVC
        case address::MHPMCOUNTERH_START ... address::MHPMCOUNTERH_END: return 0;
#endif

        case address::MVENDORID:        return 0;
        case address::MARCHID:          return 0; 
        case address::MIMPID:           return 0; 
        case address::MHARTID:          return 0;
        case address::MCONFIGPTR:       return 0;

        //TODO time, cycle, instret somewhere

        default:                        invoke_rv_exception(ILLEGAL_INSTRUCTION);
    }
}

//implicit_write must always ensure all CSRs in the class are legal
void CSR::CSR_t::implicit_write(uint16_t csr, word_t data) {//Does not perform any privilege checks
    //TODO handle WARL in this function
    //TODO workaround MSVC not supporting non-standard case ranges
    switch (csr) {
        case address::SSTATUS:          this->mstatus = data; return;//FIXME only some parts of mstatus are writable from sstatus
        case address::SIE:              this->sie = data; return;//FIXME WARL
        case address::STVEC:            this->stvec = data; return;//FIXME WARL
        case address::SCOUNTEREN:       this->scounteren = data; return;//FIXME WARL
        case address::SENVCFG:          this->senvcfg = data & 0b1; return;//TODO is this correct?
        case address::SSCRATCH:         this->sscratch = data; return;
        case address::SEPC:             this->sepc = data & 0xFFFFFFFC; return;//IALIGN=32
        case address::SCAUSE:           this->scause = data; return;//FIXME WARL
        case address::STVAL:            return;//We simply ignore writes to STVAL, NOT throw an exception
        case address::SIP:              this->sip = data; return;//FIXME WARL
        case address::SATP:             this->satp = data; return;//FIXME WARL
        case address::MSTATUS:          this->mstatus = data; return;//FIXME WARL
        case address::MISA:             return;//We simply ignore writes to MISA, NOT throw an exception
        case address::MEDELEG:          this->medeleg = data; return;//FIXME WARL
        case address::MIDELEG:          this->mideleg = data; return;//FIXME WARL
        case address::MIE:              this->mie = data; return;//FIXME WARL
        case address::MENVCFG:          this->menvcfg = data & 0b1; return;//Only lowest bit is RW
        case address::MSTATUSH:         this->mstatush = data; return;//FIXME WARL
        case address::MENVCFGH:         return;//We simply ignore writes to MENVCFGH, NOT throw an exception
        case address::MCOUNTINHIBIT:    return;//We simply ignore writes to MCOUNTINHIBIT, NOT throw an exception

#ifndef _MSC_VER//Not on MSVC
        case address::MHPMEVENT_START ... address::MHPMEVENT_END: return;//We simply ignore writes to the HPMCOUNTER CSRs, NOT throw exceptions
#endif

        case address::MSCRATCH:         this->mscratch  = data; return;
        case address::MEPC:             this->sepc = data & 0xFFFFFFFC; return;//IALIGN=32
        case address::MCAUSE:           this->mcause    = data; return;//FIXME WARL
        case address::MTVAL:                                    return;//We simply ignore writes to MTVAL, NOT throw an exception
        case address::MIP:              this->mip       = data; return;//FIXME WARL

#ifndef _MSC_VER//Not on MSVC
        case address::PMPCFG_START  ... address::PMPCFG_END:    this->pmpcfg [csr - address::PMPCFG_START]  = data; return;//FIXME WARL
        case address::PMPADDR_START ... address::PMPADDR_END:   this->pmpaddr[csr - address::PMPADDR_START] = data; return;//FIXME WARL
#endif

        case address::MCYCLE:           this->mcycle    = (this->mcycle   & 0xFFFFFFFF00000000) | ((uint64_t) data.u); return;
        case address::MINSTRET:         this->minstret  = (this->minstret & 0xFFFFFFFF00000000) | ((uint64_t) data.u); return;

#ifndef _MSC_VER//Not on MSVC
        case address::MHPMCOUNTER_START ... address::MHPMCOUNTER_END: return;//We simply ignore writes to the HPMCOUNTER CSRs, NOT throw exceptions
#endif

        case address::MCYCLEH:          this->mcycle    = (this->mcycle   & 0x00000000FFFFFFFF) | (((uint64_t) data.u) << 32); return;
        case address::MINSTRETH:        this->minstret  = (this->minstret & 0x00000000FFFFFFFF) | (((uint64_t) data.u) << 32); return;

#ifndef _MSC_VER//Not on MSVC
        case address::MHPMCOUNTERH_START ... address::MHPMCOUNTERH_END: return;//We simply ignore writes to the HPMCOUNTERH CSRs, NOT throw exceptions
#endif

        //TODO time, cycle, instret somewhere

        default:                        invoke_rv_exception(ILLEGAL_INSTRUCTION);
    }
}

void CSR::CSR_t::set_privilege_mode(privilege_mode_t new_privilege_mode) {
    this->m_privilege_mode = new_privilege_mode;
}

CSR::privilege_mode_t CSR::CSR_t::get_privilege_mode() const {
    return this->m_privilege_mode;
}

bool CSR::CSR_t::current_privilege_mode_can_explicitly_read(uint16_t csr) const {
    uint32_t min_privilege_required = (csr >> 8) & 0b11;
    return (uint32_t)(m_privilege_mode) >= min_privilege_required;
}

bool CSR::CSR_t::current_privilege_mode_can_explicitly_write(uint16_t csr) const {
    if (((csr >> 10) & 0b11) == 0b11) {//If top 2 bits are 1, then it's a read only CSR
        return false;
    } else {//Otherwise permissions are handled the same as for reading
        return this->current_privilege_mode_can_explicitly_read(csr);
    }
}
