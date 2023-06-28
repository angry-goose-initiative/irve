/* CSR.cpp
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Contains (mostly) CSRs for IRVE as well as methods to read and write them.
 *
*/

/* Constants and Defines */

//Only some bits of mstatus are accessible in S-mode
#define SSTATUS_MASK 0b10000000'00001101'11100111'01100010

/* Includes */

#include "CSR.h"

#include <cassert>
#include <chrono>
#include <cstddef>

#include "rvexception.h"

using namespace irve::internal;

/* Function Implementations */

//See Volume 2 Section 3.4
CSR::CSR_t::CSR_t() : 
    sie(0),//Only needs to be initialized for implicit_read() guarantees
    stvec(0),//Only needs to be initialized for implicit_read() guarantees
    scounteren(0),//Only needs to be initialized for implicit_read() guarantees
    senvcfg(0),//Only needs to be initialized for implicit_read() guarantees
    sscratch(0),//We don't need to initialize this since all states are valid, but sanitizers could complain otherwise
    sepc(0),//Only needs to be initialized for implicit_read() guarantees
    scause(0),//Only needs to be initialized for implicit_read() guarantees
    sip(0),//Only needs to be initialized for implicit_read() guarantees
    satp(0),//Only needs to be initialized for implicit_read() guarantees
    mstatus(0),//MUST BE INITIALIZED ACCORDING TO THE SPEC//FIXME is this the correct starting value??
    medeleg(0),//Only needs to be initialized for implicit_read() guarantees
    mideleg(0),//Only needs to be initialized for implicit_read() guarantees
    mie(0),//Only needs to be initialized for implicit_read() guarantees (also good to have interrupts disabled by default)
    menvcfg(0),//Only needs to be initialized for implicit_read() guarantees
    mscratch(0),//We don't need to initialize this since all states are valid, but sanitizers could complain otherwise
    mepc(0),//Only needs to be initialized for implicit_read() guarantees
    mcause(0),//MUST BE INITIALIZED ACCORDING TO THE SPEC (we don't distinguish reset conditions, so we just use 0 here)
    mip(0),//Only needs to be initialized for implicit_read() guarantees
    minstret(0),//Implied it should be initialized according to the spec
    mcycle(0),//Implied it should be initialized according to the spec
    mtime(0),//Implied it should be initialized according to the spec
    mtimecmp(0xFFFFFFFFFFFFFFFF),//Implied it should be initialized according to the spec
    m_last_time_update(std::chrono::steady_clock::now()),
    m_delay_update_counter(0),
    m_privilege_mode(CSR::privilege_mode_t::MACHINE_MODE)//MUST BE INITIALIZED ACCORDING TO THE SPEC
{}

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
    //FIXME workaround MSVC not supporting non-standard case ranges

    switch (csr) {
        case address::SSTATUS:          return this->mstatus & SSTATUS_MASK;//Only some bits of mstatus are accessible in S-mode
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
        case address::MENVCFG:          return this->menvcfg;
        case address::MSTATUSH:         return 0;//We only support little-endian
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

        case address::MCYCLE:           return (uint32_t)(this->mcycle      & 0xFFFFFFFF);
        case address::MINSTRET:         return (uint32_t)(this->minstret    & 0xFFFFFFFF);

#ifndef _MSC_VER//Not on MSVC
        case address::MHPMCOUNTER_START ... address::MHPMCOUNTER_END: return 0;
#endif

        case address::MCYCLEH:          return (uint32_t)((this->mcycle     >> 32) & 0xFFFFFFFF);
        case address::MINSTRETH:        return (uint32_t)((this->minstret   >> 32) & 0xFFFFFFFF);

#ifndef _MSC_VER//Not on MSVC
        case address::MHPMCOUNTERH_START ... address::MHPMCOUNTERH_END: return 0;
#endif

        case address::MTIME:            return (uint32_t)(this->mtime            & 0xFFFFFFFF);//Custom
        case address::MTIMEH:           return (uint32_t)((this->mtime    >> 32) & 0xFFFFFFFF);//Custom
        case address::MTIMECMP:         return (uint32_t)(this->mtimecmp         & 0xFFFFFFFF);//Custom
        case address::MTIMECMPH:        return (uint32_t)((this->mtimecmp >> 32) & 0xFFFFFFFF);//Custom

        case address::CYCLE:            return this->implicit_read(address::MCYCLE);
        case address::TIME:             return this->implicit_read(address::MTIME);
        case address::INSTRET:          return this->implicit_read(address::MINSTRET);

#ifndef _MSC_VER//Not on MSVC
        case address::HPMCOUNTER_START ... address::HPMCOUNTER_END: return 0;
#endif

        case address::CYCLEH:           return this->implicit_read(address::MCYCLEH);
        case address::TIMEH:            return this->implicit_read(address::MTIMEH);
        case address::INSTRETH:         return this->implicit_read(address::MINSTRETH);

#ifndef _MSC_VER//Not on MSVC
        case address::HPMCOUNTERH_START ... address::HPMCOUNTERH_END: return 0;
#endif

        case address::MVENDORID:        return 0;
        case address::MARCHID:          return 0; 
        case address::MIMPID:           return 0; 
        case address::MHARTID:          return 0;
        case address::MCONFIGPTR:       return 0;

        default:                        invoke_rv_exception(ILLEGAL_INSTRUCTION);
    }
}

//implicit_write must always ensure all CSRs in the class are legal
void CSR::CSR_t::implicit_write(uint16_t csr, word_t data) {//Does not perform any privilege checks
    //FIXME handle WARL in this function

    //TODO workaround MSVC not supporting non-standard case ranges
    switch (csr) {
        case address::SSTATUS:          this->mstatus = (this->mstatus & ~SSTATUS_MASK) | (data & SSTATUS_MASK); return;//Only some parts of mstatus are writable from sstatus
        case address::SIE:              this->sie = data; return;//FIXME WARL
        case address::STVEC:            this->stvec = data; return;//FIXME WARL
        case address::SCOUNTEREN:       this->scounteren = data; return;//FIXME WARL
        case address::SENVCFG:          this->senvcfg = data & 0b1; return;//Only lowest bit is RW
        case address::SSCRATCH:         this->sscratch = data; return;
        case address::SEPC:             this->sepc = data & 0xFFFFFFFC; return;//IALIGN=32
        case address::SCAUSE:           this->scause = data; return;//FIXME WARL
        case address::STVAL:            return;//We simply ignore writes to STVAL, NOT throw an exception
        case address::SIP:              this->sip = data; return;//FIXME WARL
        case address::SATP:             this->satp = data; return;//FIXME WARL
        case address::MSTATUS:          this->mstatus = data; return;//FIXME WARL (less critical assuming safe M-mode code)
        case address::MISA:             return;//We simply ignore writes to MISA, NOT throw an exception
        case address::MEDELEG:          this->medeleg = data & 0b0000000000000000'1011001111111111; return;//Note it dosn't make sense to delegate ECALL from M-mode since we can never delagte to high levels
        case address::MIDELEG:          this->mideleg = data & 0b00000000000000000000'1010'1010'1010; return;
        case address::MIE:              this->mie     = data & 0b00000000000000000000'1010'1010'1010; return;
        case address::MENVCFG:          this->menvcfg = data & 0b1; return;//Only lowest bit is RW
        case address::MSTATUSH:         return;//We simply ignore writes to mstatush, NOT throw an exception
        case address::MENVCFGH:         return;//We simply ignore writes to menvcfgh, NOT throw an exception
        case address::MCOUNTINHIBIT:    return;//We simply ignore writes to mcountinhibit, NOT throw an exception

#ifndef _MSC_VER//Not on MSVC
        case address::MHPMEVENT_START ... address::MHPMEVENT_END: return;//We simply ignore writes to the HPMCOUNTER CSRs, NOT throw exceptions
#endif

        case address::MSCRATCH:         this->mscratch  = data;                 return;
        case address::MEPC:             this->mepc      = data & 0xFFFFFFFC;    return;//IALIGN=32
        case address::MCAUSE:           this->mcause    = data;                 return;//FIXME WARL
        case address::MTVAL:                                                    return;//We simply ignore writes to MTVAL, NOT throw an exception
        case address::MIP:              this->mip       = data & 0b00000000000000000000'0010'0010'0010; return;//Note ALL interrupt pending bits for M-mode are READ ONLY

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

        case address::MTIME:            this->mtime     = (this->mtime    & 0xFFFFFFFF00000000) | ((uint64_t)  data.u);         return;//Custom
        case address::MTIMEH:           this->mtime     = (this->mtime    & 0x00000000FFFFFFFF) | (((uint64_t) data.u) << 32);  return;//Custom
        case address::MTIMECMP://Custom
            this->mtimecmp  = (this->mtimecmp & 0xFFFFFFFF00000000) | ((uint64_t)  data.u);
            this->mip &= ~(1 << 7);//Clear mip.MTIP on writes to mtimecmp (which would normally be in memory, but we made it a CSR so might as well handle it here)
            return;
        case address::MTIMECMPH://Custom
            this->mtimecmp  = (this->mtimecmp & 0x00000000FFFFFFFF) | (((uint64_t) data.u) << 32);
            this->mip &= ~(1 << 7);//Clear mip.MTIP on writes to mtimecmp (which would normally be in memory, but we made it a CSR so might as well handle it here)
            return;

        default:                        invoke_rv_exception(ILLEGAL_INSTRUCTION);
    }
}

void CSR::CSR_t::set_privilege_mode(privilege_mode_t new_privilege_mode) {
    this->m_privilege_mode = new_privilege_mode;
}

CSR::privilege_mode_t CSR::CSR_t::get_privilege_mode() const {
    return this->m_privilege_mode;
}

void CSR::CSR_t::update_timer() {
    //Only actually check if we should increment the timer every 65535 times this function is called
    //This is since chrono is REALLY REALLY REALLY slow
    ++m_delay_update_counter;
    if (m_delay_update_counter) return;//Counter didn't overflow, so don't update the timer

    //This is really, really slow. Like, we couldn't even run at 1MHz if we did this every time
    std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
    double last_time_update_us = std::chrono::duration_cast<std::chrono::microseconds>(now - this->m_last_time_update).count();
    if (last_time_update_us > 1000) {//1ms
        this->m_last_time_update = now;
        ++(this->mtime);
        if (this->mtime >= this->mtimecmp) {
            this->mip |= 1 << 7;//Set the machine timer interrupt as pending
        }
    }
}

bool CSR::CSR_t::current_privilege_mode_can_explicitly_read(uint16_t csr) const {
    //FIXME special checks for cycle, instret, time, and hpmcounters

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
