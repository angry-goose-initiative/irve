/**
 * @brief   Contains (mostly) CSRs for IRVE as well as methods to read and write them.
 * 
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  Copyright (C) 2023 Nick Chan\n
 *  See the LICENSE file at the root of the project for licensing info.
*/

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include "csr.h"

#include <cassert>
#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <cstring>

#include "rv_trap.h"

#include "fuzzish.h"

using namespace irve::internal;

/* ------------------------------------------------------------------------------------------------
 * Constants/Defines
 * --------------------------------------------------------------------------------------------- */

//Only some bits of mstatus are accessible in S-mode
#define SSTATUS_MASK    0b10000000'00001101'11100111'01100010
#define SIP_MASK        0b00000000'00000000'00000010'00100010
#define SIE_MASK        0b00000000'00000000'00000010'00100010
#define SATP_MASK       0b1'000000000'1111111111111111111111
//TODO actually implement MISA and friends at some point
//                                   ABCDEFGHIJKLMNOPQRSTUVWXYZ
//#define MISA_CONTENTS Word(0b01000010000000100010000010100100)

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

//See Volume 2 Section 3.4
Csr::Csr() :
    stvec(0),                       //Only needs to be initialized for implicit_read() guarantees
    scounteren(0),                  //Only needs to be initialized for implicit_read() guarantees
    senvcfg(0),                     //Only needs to be initialized for implicit_read() guarantees
    sscratch(irve_fuzzish_rand()),  //We don't need to initialize this since all states are valid, but sanitizers could complain otherwise
    sepc(0),                        //Only needs to be initialized for implicit_read() guarantees
    scause(0),                      //Only needs to be initialized for implicit_read() guarantees
    satp(0),                        //Only needs to be initialized for implicit_read() guarantees
    mstatus(0),                     //MUST BE INITIALIZED ACCORDING TO THE SPEC//FIXME is this the correct starting value??
    medeleg(0),                     //Only needs to be initialized for implicit_read() guarantees
    mideleg(0),                     //Only needs to be initialized for implicit_read() guarantees
    mie(0),                         //Only needs to be initialized for implicit_read() guarantees (also good to have interrupts disabled by default)
    mtvec(0x00000004 | 0b01),       //Doesn't need to be initialized, but this is convenient for RVSW
    menvcfg(0),                     //Only needs to be initialized for implicit_read() guarantees
    mscratch(irve_fuzzish_rand()),  //We don't need to initialize this since all states are valid, but sanitizers could complain otherwise
    mepc(0),                        //Only needs to be initialized for implicit_read() guarantees
    mcause(0),                      //MUST BE INITIALIZED ACCORDING TO THE SPEC (we don't distinguish reset conditions, so we just use 0 here)
    mip(0),                         //Only needs to be initialized for implicit_read() guarantees
    //PMPCFG and PMPADDR registers done in the constructor's body
    minstret(0),                    //Implied it should be initialized according to the spec
    mcycle(0),                      //Implied it should be initialized according to the spec
    mtime(0),                       //Implied it should be initialized according to the spec
    mtimecmp(0xFFFFFFFFFFFFFFFF),   //Implied it should be initialized according to the spec
    m_last_time_update(std::chrono::steady_clock::now()),
    m_delay_update_counter(0),
    m_privilege_mode(PrivilegeMode::MACHINE_MODE) //MUST BE INITIALIZED ACCORDING TO THE SPEC
{
    std::memset(this->pmpcfg, 0x00, sizeof(this->pmpcfg)); // We need the A and L bits to be 0

    // We don't need to initialize this since all states are valid, but sanitizers could complain otherwise
    irve_fuzzish_meminit(this->pmpaddr, sizeof(this->pmpaddr));
}

Reg Csr::explicit_read(Csr::Address csr) {//Performs privilege checks
    if (!this->current_privilege_mode_can_explicitly_read(csr))
        rv_trap::invoke_exception(rv_trap::Cause::ILLEGAL_INSTRUCTION_EXCEPTION);
    
    return this->implicit_read(csr);
}

void Csr::explicit_write(Csr::Address csr, Word data) {//Performs privilege checks
    if (!this->current_privilege_mode_can_explicitly_write(csr)) {
        rv_trap::invoke_exception(rv_trap::Cause::ILLEGAL_INSTRUCTION_EXCEPTION);
    } else {
        this->implicit_write(csr, data);
    }
}

//We assume the CSRs within the class are "safe" for the purposes of reads
Reg Csr::implicit_read(Csr::Address csr) {//Does not perform any privilege checks
    switch (csr) {
        case Csr::Address::SSTATUS:          return this->mstatus & SSTATUS_MASK;//Only some bits of mstatus are accessible in S-mode
        case Csr::Address::SIE:              return this->mie & SIE_MASK;//Only some bits of mie are accessible in S-mode
        case Csr::Address::STVEC:            return this->stvec;
        case Csr::Address::SCOUNTEREN:       return this->scounteren;
        case Csr::Address::SENVCFG:          return this->senvcfg;
        case Csr::Address::SSCRATCH:         return this->sscratch;
        case Csr::Address::SEPC:             return this->sepc;
        case Csr::Address::SCAUSE:           return this->scause;
        case Csr::Address::STVAL:            return this->stval;
        case Csr::Address::SIP:              return this->mip & SIP_MASK;//Only some bits of mip are accessible in S-mode
        case Csr::Address::SATP:             return this->satp;
        case Csr::Address::MSTATUS:          return this->mstatus;
        case Csr::Address::MISA:             return 0;
        case Csr::Address::MEDELEG:          return this->medeleg;
        case Csr::Address::MIDELEG:          return this->mideleg;
        case Csr::Address::MIE:              return this->mie;
        case Csr::Address::MTVEC:            return this->mtvec;
        case Csr::Address::MCOUNTEREN:       return 0;//Since we chose to make this 0, we don't need to implement any user-mode-facing counters
        case Csr::Address::MENVCFG:          return this->menvcfg;
        case Csr::Address::MSTATUSH:         return 0;//We only support little-endian
        case Csr::Address::MENVCFGH:         return 0;
        case Csr::Address::MCOUNTINHIBIT:    return 0;

        case Csr::Address::MHPMEVENT_START ... Csr::Address::MHPMEVENT_END: return 0;

        case Csr::Address::MSCRATCH:         return this->mscratch;
        case Csr::Address::MEPC:             return this->mepc;
        case Csr::Address::MCAUSE:           return this->mcause;
        case Csr::Address::MTVAL:            return 0;
        case Csr::Address::MIP:              return this->mip;

        case Csr::Address::PMPCFG_START  ... Csr::Address::PMPCFG_END:    return this->pmpcfg [static_cast<uint16_t>(csr) - static_cast<uint16_t>(Csr::Address::PMPCFG_START)];
        case Csr::Address::PMPADDR_START ... Csr::Address::PMPADDR_END:   return this->pmpaddr[static_cast<uint16_t>(csr) - static_cast<uint16_t>(Csr::Address::PMPADDR_START)];

        case Csr::Address::MCYCLE:           return (uint32_t)(this->mcycle      & 0xFFFFFFFF);
        case Csr::Address::MINSTRET:         return (uint32_t)(this->minstret    & 0xFFFFFFFF);

        case Csr::Address::MHPMCOUNTER_START ... Csr::Address::MHPMCOUNTER_END: return 0;

        case Csr::Address::MCYCLEH:          return (uint32_t)((this->mcycle     >> 32) & 0xFFFFFFFF);
        case Csr::Address::MINSTRETH:        return (uint32_t)((this->minstret   >> 32) & 0xFFFFFFFF);

        case Csr::Address::MHPMCOUNTERH_START ... Csr::Address::MHPMCOUNTERH_END: return 0;

        case Csr::Address::MTIME:            this->update_timer(); return (uint32_t)(this->mtime            & 0xFFFFFFFF);//Custom
        case Csr::Address::MTIMEH:           this->update_timer(); return (uint32_t)((this->mtime    >> 32) & 0xFFFFFFFF);//Custom
        case Csr::Address::MTIMECMP:         return (uint32_t)(this->mtimecmp         & 0xFFFFFFFF);//Custom
        case Csr::Address::MTIMECMPH:        return (uint32_t)((this->mtimecmp >> 32) & 0xFFFFFFFF);//Custom

        case Csr::Address::CYCLE:            return this->implicit_read(Csr::Address::MCYCLE);
        case Csr::Address::TIME:             return this->implicit_read(Csr::Address::MTIME);
        case Csr::Address::INSTRET:          return this->implicit_read(Csr::Address::MINSTRET);

        case Csr::Address::HPMCOUNTER_START ... Csr::Address::HPMCOUNTER_END: return 0;

        case Csr::Address::CYCLEH:           return this->implicit_read(Csr::Address::MCYCLEH);
        case Csr::Address::TIMEH:            return this->implicit_read(Csr::Address::MTIMEH);
        case Csr::Address::INSTRETH:         return this->implicit_read(Csr::Address::MINSTRETH);

        case Csr::Address::HPMCOUNTERH_START ... Csr::Address::HPMCOUNTERH_END: return 0;

        case Csr::Address::MVENDORID:        return 0;
        case Csr::Address::MARCHID:          return 0; 
        case Csr::Address::MIMPID:           return 0; 
        case Csr::Address::MHARTID:          return 0;
        case Csr::Address::MCONFIGPTR:       return 0;

        default: rv_trap::invoke_exception(rv_trap::Cause::ILLEGAL_INSTRUCTION_EXCEPTION);
    }

    return 0; // Avoid no return warning
}

//implicit_write must always ensure all CSRs in the class are legal
void Csr::implicit_write(Csr::Address csr, Word data) {//Does not perform any privilege checks
    //FIXME handle WARL in this function

    switch (csr) {
        case Csr::Address::SSTATUS:          this->mstatus = (this->mstatus & ~SSTATUS_MASK) | (data & SSTATUS_MASK); return;//Only some parts of mstatus are writable from sstatus
        case Csr::Address::SIE:              this->mie = (this->mie & ~SIE_MASK) | (data & SIE_MASK); return;//Only some parts of mie are writable from sie
        case Csr::Address::STVEC:            this->stvec = data; return;//FIXME WARL
        case Csr::Address::SCOUNTEREN:       this->scounteren = data; return;//FIXME WARL
        case Csr::Address::SENVCFG:          this->senvcfg = data & 0b1; return;//Only lowest bit is RW
        case Csr::Address::SSCRATCH:         this->sscratch = data; return;
        case Csr::Address::SEPC:             this->sepc = data & 0xFFFFFFFC; return;//IALIGN=32
        case Csr::Address::SCAUSE:           this->scause = data; return;//FIXME WARL
        case Csr::Address::STVAL:            this->stval = data; return;//FIXME WARL
        case Csr::Address::SIP:              this->mip = (this->mip & ~SIP_MASK) | (data & SIP_MASK); return;//Only some parts of mip are writable from sip
        case Csr::Address::SATP:             this->satp = data & SATP_MASK; return;//ASIDs are unsupported
        case Csr::Address::MSTATUS:          this->mstatus = data; return;//FIXME WARL (less critical assuming safe M-mode code)
        case Csr::Address::MISA:             return;//We simply ignore writes to MISA, NOT throw an exception
        case Csr::Address::MEDELEG:          this->medeleg = data & 0b0000000000000000'1011001111111111; return;//Note it dosn't make sense to delegate ECALL from M-mode since we can never delagte to high levels
        case Csr::Address::MIDELEG:          this->mideleg = data & 0b00000000000000000000'1010'1010'1010; return;
        case Csr::Address::MIE:              this->mie     = data & 0b00000000000000000000'1010'1010'1010; return;
        case Csr::Address::MTVEC:            this->mtvec   = data; return;//FIXME WARL
        case Csr::Address::MENVCFG:          this->menvcfg = data & 0b1; return;//Only lowest bit is RW
        case Csr::Address::MSTATUSH:         return;//We simply ignore writes to mstatush, NOT throw an exception
        case Csr::Address::MENVCFGH:         return;//We simply ignore writes to menvcfgh, NOT throw an exception
        case Csr::Address::MCOUNTINHIBIT:    return;//We simply ignore writes to mcountinhibit, NOT throw an exception

        case Csr::Address::HPMCOUNTER_START ... Csr::Address::HPMCOUNTER_END: return;//We simply ignore writes to the HPMCOUNTER CSRs, NOT throw exceptions

        case Csr::Address::MSCRATCH:         this->mscratch  = data;                 return;
        case Csr::Address::MEPC:             this->mepc      = data & 0xFFFFFFFC;    return;//IALIGN=32
        case Csr::Address::MCAUSE:           this->mcause    = data;                 return;//FIXME WARL
        case Csr::Address::MTVAL:                                                    return;//We simply ignore writes to MTVAL, NOT throw an exception
        case Csr::Address::MIP:              this->mip       = data & 0b00000000000000000000'0010'0010'0010; return;//Note ALL interrupt pending bits for M-mode are READ ONLY

        //FIXME when locked, ignore (not throw exception) on writes to the relevant PMP CSRs
        case Csr::Address::PMPCFG_START  ... Csr::Address::PMPCFG_END:    this->pmpcfg [static_cast<uint16_t>(csr) - static_cast<uint16_t>(Csr::Address::PMPCFG_START)] = data; return;//FIXME WARL
        case Csr::Address::PMPADDR_START ... Csr::Address::PMPADDR_END:   this->pmpaddr[static_cast<uint16_t>(csr) - static_cast<uint16_t>(Csr::Address::PMPADDR_START)] = data; return;//FIXME WARL

        case Csr::Address::MCYCLE:           this->mcycle    = (this->mcycle   & 0xFFFFFFFF00000000) | ((uint64_t) data.u); return;
        case Csr::Address::MINSTRET:         this->minstret  = (this->minstret & 0xFFFFFFFF00000000) | ((uint64_t) data.u); return;

        case Csr::Address::MHPMCOUNTER_START ... Csr::Address::MHPMCOUNTER_END: return;//We simply ignore writes to the HPMCOUNTER CSRs, NOT throw exceptions

        case Csr::Address::MCYCLEH:          this->mcycle    = (this->mcycle   & 0x00000000FFFFFFFF) | (((uint64_t) data.u) << 32); return;
        case Csr::Address::MINSTRETH:        this->minstret  = (this->minstret & 0x00000000FFFFFFFF) | (((uint64_t) data.u) << 32); return;

        case Csr::Address::MHPMCOUNTERH_START ... Csr::Address::MHPMCOUNTERH_END: return;//We simply ignore writes to the HPMCOUNTERH CSRs, NOT throw exceptions

        case Csr::Address::MTIME: {//Custom
            this->mtime     = (this->mtime    & 0xFFFFFFFF00000000) | ((uint64_t)  data.u);
            std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
            this->m_last_time_update = now;
            return;
        }
        case Csr::Address::MTIMEH: {//Custom
            this->mtime     = (this->mtime    & 0x00000000FFFFFFFF) | (((uint64_t) data.u) << 32);
            std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
            this->m_last_time_update = now;
            return;
        }
        case Csr::Address::MTIMECMP://Custom
            this->mtimecmp  = (this->mtimecmp & 0xFFFFFFFF00000000) | ((uint64_t)  data.u);
            this->mip &= ~(1 << 7);//Clear mip.MTIP on writes to mtimecmp (which would normally be in memory, but we made it a CSR so might as well handle it here)
            return;
        case Csr::Address::MTIMECMPH://Custom
            this->mtimecmp  = (this->mtimecmp & 0x00000000FFFFFFFF) | (((uint64_t) data.u) << 32);
            this->mip &= ~(1 << 7);//Clear mip.MTIP on writes to mtimecmp (which would normally be in memory, but we made it a CSR so might as well handle it here)
            return;

        default: rv_trap::invoke_exception(rv_trap::Cause::ILLEGAL_INSTRUCTION_EXCEPTION);
    }
}

void Csr::set_privilege_mode(PrivilegeMode new_privilege_mode) {
    this->m_privilege_mode = new_privilege_mode;
}

PrivilegeMode Csr::get_privilege_mode() const {
    return this->m_privilege_mode;
}

void Csr::update_timer() {
    //This is really, really slow
    //TODO make this function faster
    std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
    double time_since_last_update_us = std::chrono::duration_cast<std::chrono::microseconds>(now - this->m_last_time_update).count();
    if (time_since_last_update_us > 1000.0) {//1ms
        this->m_last_time_update = now;
        //++(this->mtime);
        this->mtime += (uint32_t)(time_since_last_update_us / 1000.0);//FIXME is it okay to skip values like this if we're behind?
    }

    //If the timer has passed the comparison value, cause an interrupt
    if (this->mtime >= this->mtimecmp) {
        this->mip |= 1 << 7;//Set the machine timer interrupt as pending
    }
}

void Csr::occasional_update_timer() {
    //Only actually update the timer every 65535 times this function is called
    //This is since chrono is REALLY REALLY REALLY slow
    //FIXME this however reduces the accuracy of the timer unless software is constantly checking mtime and causing update_timer() to be called more often!
    ++m_delay_update_counter;
    if (m_delay_update_counter) return;//Counter didn't overflow, so don't update the timer

    //This is really, really slow. Like, we couldn't even run at 1MHz if we did this every time
    update_timer();
}

void Csr::set_exti_pending() {
    this->mip |= 1 << 11;//Set the machine external interrupt as pending
}

bool Csr::current_privilege_mode_can_explicitly_read(Csr::Address csr) const {
    //FIXME special checks for cycle, instret, time, and hpmcounters

    uint32_t min_privilege_required = (static_cast<uint16_t>(csr) >> 8) & 0b11;
    return (uint32_t)(m_privilege_mode) >= min_privilege_required;
}

bool Csr::current_privilege_mode_can_explicitly_write(Csr::Address csr) const {
    if (((static_cast<uint16_t>(csr) >> 10) & 0b11) == 0b11) {//If top 2 bits are 1, then it's a read only CSR
        return false;
    } else {//Otherwise permissions are handled the same as for reading
        return this->current_privilege_mode_can_explicitly_read(csr);
    }
}
