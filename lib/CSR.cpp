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

#include "CSR.h"

#include <cassert>
#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <cstring>

#include "rvexception.h"

#include "fuzzish.h"

using namespace irve::internal;

/* ------------------------------------------------------------------------------------------------
 * Constants/Defines
 * --------------------------------------------------------------------------------------------- */

//Only some bits of mstatus are accessible in S-mode
#define SSTATUS_MASK 0b10000000'00001101'11100111'01100010

//TODO actually implement MISA and friends at some point
//                                   ABCDEFGHIJKLMNOPQRSTUVWXYZ
//#define MISA_CONTENTS Word(0b01000010000000100010000010100100)

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

//See Volume 2 Section 3.4
Csr::Csr()
    : sie(0) //Only needs to be initialized for implicit_read() guarantees
    , stvec(0) //Only needs to be initialized for implicit_read() guarantees
    , scounteren(0) //Only needs to be initialized for implicit_read() guarantees
    , senvcfg(0) //Only needs to be initialized for implicit_read() guarantees
    , sscratch(irve_fuzzish_rand()) //We don't need to initialize this since all states are valid, but sanitizers could complain otherwise
    , sepc(0) //Only needs to be initialized for implicit_read() guarantees
    , scause(0) //Only needs to be initialized for implicit_read() guarantees
    , sip(0) //Only needs to be initialized for implicit_read() guarantees
    , satp(0) //Only needs to be initialized for implicit_read() guarantees
    , mstatus(0) //MUST BE INITIALIZED ACCORDING TO THE SPEC//FIXME is this the correct starting value??
    , medeleg(0) //Only needs to be initialized for implicit_read() guarantees
    , mideleg(0) //Only needs to be initialized for implicit_read() guarantees
    , mie(0) //Only needs to be initialized for implicit_read() guarantees (also good to have interrupts disabled by default)
    , mtvec(0x00000004 | 0b01) //Doesn't need to be initialized, but this is convenient for RVSW
    , menvcfg(0) //Only needs to be initialized for implicit_read() guarantees
    , mscratch(irve_fuzzish_rand()) //We don't need to initialize this since all states are valid, but sanitizers could complain otherwise
    , mepc(0) //Only needs to be initialized for implicit_read() guarantees
    , mcause(0) //MUST BE INITIALIZED ACCORDING TO THE SPEC (we don't distinguish reset conditions, so we just use 0 here)
    , mip(0) //Only needs to be initialized for implicit_read() guarantees
      //PMPCFG and PMPADDR registers done below
    , minstret(0) //Implied it should be initialized according to the spec
    , mcycle(0) //Implied it should be initialized according to the spec
    , mtime(0) //Implied it should be initialized according to the spec
    , mtimecmp(0xFFFFFFFFFFFFFFFF) //Implied it should be initialized according to the spec
    , m_last_time_update(std::chrono::steady_clock::now())
    , m_delay_update_counter(0)
    , m_privilege_mode(PrivilegeMode::MACHINE_MODE) //MUST BE INITIALIZED ACCORDING TO THE SPEC
{
    std::memset(this->pmpcfg, 0x00, sizeof(this->pmpcfg)); // We need the A and L bits to be 0

    // We don't need to initialize this since all states are valid, but sanitizers could complain otherwise
    irve_fuzzish_meminit(this->pmpaddr, sizeof(this->pmpaddr));
}

Reg Csr::explicit_read(Csr::Address csr) const {//Performs privilege checks
    if (!this->current_privilege_mode_can_explicitly_read(csr)) {
        invoke_rv_exception(ILLEGAL_INSTRUCTION);
    } else {
        return this->implicit_read(csr);
    }
}

void Csr::explicit_write(Csr::Address csr, Word data) {//Performs privilege checks
    if (!this->current_privilege_mode_can_explicitly_write(csr)) {
        invoke_rv_exception(ILLEGAL_INSTRUCTION);
    } else {
        this->implicit_write(csr, data);
    }
}

//We assume the CSRs within the class are "safe" for the purposes of reads
Reg Csr::implicit_read(Csr::Address csr) const {//Does not perform any privilege checks
    switch (csr) {
        case Csr::Address::kSstatus:          return this->mstatus & SSTATUS_MASK;//Only some bits of mstatus are accessible in S-mode
        case Csr::Address::kSie:              return this->sie;
        case Csr::Address::kStvec:            return this->stvec;
        case Csr::Address::kScounteren:       return this->scounteren;
        case Csr::Address::kSenvcfg:          return this->senvcfg;
        case Csr::Address::kSscratch:         return this->sscratch;
        case Csr::Address::kSepc:             return this->sepc;
        case Csr::Address::kScause:           return this->scause;
        case Csr::Address::kStval:            return 0;
        case Csr::Address::kSip:              return this->sip;
        case Csr::Address::kSatp:             return this->satp;
        case Csr::Address::kMstatus:          return this->mstatus;
        case Csr::Address::kMisa:             return 0;
        case Csr::Address::kMedeleg:          return this->medeleg;
        case Csr::Address::kMideleg:          return this->mideleg;
        case Csr::Address::kMie:              return this->mie;
        case Csr::Address::kMtvec:            return this->mtvec;
        case Csr::Address::kMcounteren:       return 0;//Since we chose to make this 0, we don't need to implement any user-mode-facing counters
        case Csr::Address::kMenvcfg:          return this->menvcfg;
        case Csr::Address::kMstatush:         return 0;//We only support little-endian
        case Csr::Address::kMenvcfgh:         return 0;
        case Csr::Address::kMcountinhibit:    return 0;

        case Csr::Address::kMhpmeventStart ... Csr::Address::kMhpmeventEnd: return 0;

        case Csr::Address::kMscratch:         return this->mscratch;
        case Csr::Address::kMepc:             return this->mepc;
        case Csr::Address::kMcause:           return this->mcause;
        case Csr::Address::kMtval:            return 0;
        case Csr::Address::kMip:              return this->mip;

        case Csr::Address::kPmpcfgStart  ... Csr::Address::kPmpcfgEnd:    return this->pmpcfg [static_cast<uint16_t>(csr) - static_cast<uint16_t>(Csr::Address::kPmpcfgStart)];
        case Csr::Address::kPmpaddrStart ... Csr::Address::kPmpaddrEnd:   return this->pmpaddr[static_cast<uint16_t>(csr) - static_cast<uint16_t>(Csr::Address::kPmpaddrStart)];

        case Csr::Address::kMcycle:           return (uint32_t)(this->mcycle      & 0xFFFFFFFF);
        case Csr::Address::kMinstret:         return (uint32_t)(this->minstret    & 0xFFFFFFFF);

        case Csr::Address::kMhpmcounterStart ... Csr::Address::kMhpmcounterEnd: return 0;

        case Csr::Address::kMcycleh:          return (uint32_t)((this->mcycle     >> 32) & 0xFFFFFFFF);
        case Csr::Address::kMinstreth:        return (uint32_t)((this->minstret   >> 32) & 0xFFFFFFFF);

        case Csr::Address::kMhpmcounterhStart ... Csr::Address::kMhpmcounterhEnd: return 0;

        case Csr::Address::kMtime:            return (uint32_t)(this->mtime            & 0xFFFFFFFF);//Custom
        case Csr::Address::kMtimeh:           return (uint32_t)((this->mtime    >> 32) & 0xFFFFFFFF);//Custom
        case Csr::Address::kMtimecmp:         return (uint32_t)(this->mtimecmp         & 0xFFFFFFFF);//Custom
        case Csr::Address::kMtimecmph:        return (uint32_t)((this->mtimecmp >> 32) & 0xFFFFFFFF);//Custom

        case Csr::Address::kCycle:            return this->implicit_read(Csr::Address::kMcycle);
        case Csr::Address::kTime:             return this->implicit_read(Csr::Address::kMtime);
        case Csr::Address::kInstret:          return this->implicit_read(Csr::Address::kMinstret);

        case Csr::Address::kHpmcounterStart ... Csr::Address::kHpmcounterStart: return 0;

        case Csr::Address::kCycleh:           return this->implicit_read(Csr::Address::kMcycleh);
        case Csr::Address::kTimeh:            return this->implicit_read(Csr::Address::kMtimeh);
        case Csr::Address::kInstreth:         return this->implicit_read(Csr::Address::kMinstreth);

        case Csr::Address::kHpmcounterhStart ... Csr::Address::kHpmcounterhEnd: return 0;

        case Csr::Address::kMvendorid:        return 0;
        case Csr::Address::kMarchid:          return 0; 
        case Csr::Address::kMimpid:           return 0; 
        case Csr::Address::kMhartid:          return 0;
        case Csr::Address::kMconfigptr:       return 0;

        default:                        invoke_rv_exception(ILLEGAL_INSTRUCTION);
    }
}

//implicit_write must always ensure all CSRs in the class are legal
void Csr::implicit_write(Csr::Address csr, Word data) {//Does not perform any privilege checks
    //FIXME handle WARL in this function

    switch (csr) {
        case Csr::Address::kSstatus:          this->mstatus = (this->mstatus & ~SSTATUS_MASK) | (data & SSTATUS_MASK); return;//Only some parts of mstatus are writable from sstatus
        case Csr::Address::kSie:              this->sie = data; return;//FIXME WARL
        case Csr::Address::kStvec:            this->stvec = data; return;//FIXME WARL
        case Csr::Address::kScounteren:       this->scounteren = data; return;//FIXME WARL
        case Csr::Address::kSenvcfg:          this->senvcfg = data & 0b1; return;//Only lowest bit is RW
        case Csr::Address::kSscratch:         this->sscratch = data; return;
        case Csr::Address::kSepc:             this->sepc = data & 0xFFFFFFFC; return;//IALIGN=32
        case Csr::Address::kScause:           this->scause = data; return;//FIXME WARL
        case Csr::Address::kStval:            return;//We simply ignore writes to STVAL, NOT throw an exception
        case Csr::Address::kSip:              this->sip = data; return;//FIXME WARL
        case Csr::Address::kSatp:             this->satp = data; return;//FIXME WARL
        case Csr::Address::kMstatus:          this->mstatus = data; return;//FIXME WARL (less critical assuming safe M-mode code)
        case Csr::Address::kMisa:             return;//We simply ignore writes to MISA, NOT throw an exception
        case Csr::Address::kMedeleg:          this->medeleg = data & 0b0000000000000000'1011001111111111; return;//Note it dosn't make sense to delegate ECALL from M-mode since we can never delagte to high levels
        case Csr::Address::kMideleg:          this->mideleg = data & 0b00000000000000000000'1010'1010'1010; return;
        case Csr::Address::kMie:              this->mie     = data & 0b00000000000000000000'1010'1010'1010; return;
        case Csr::Address::kMtvec:            this->mtvec   = data; return;//FIXME WARL
        case Csr::Address::kMenvcfg:          this->menvcfg = data & 0b1; return;//Only lowest bit is RW
        case Csr::Address::kMstatush:         return;//We simply ignore writes to mstatush, NOT throw an exception
        case Csr::Address::kMenvcfgh:         return;//We simply ignore writes to menvcfgh, NOT throw an exception
        case Csr::Address::kMcountinhibit:    return;//We simply ignore writes to mcountinhibit, NOT throw an exception

        case Csr::Address::kMhpmeventStart ... Csr::Address::kMhpmeventEnd: return;//We simply ignore writes to the HPMCOUNTER CSRs, NOT throw exceptions

        case Csr::Address::kMscratch:         this->mscratch  = data;                 return;
        case Csr::Address::kMepc:             this->mepc      = data & 0xFFFFFFFC;    return;//IALIGN=32
        case Csr::Address::kMcause:           this->mcause    = data;                 return;//FIXME WARL
        case Csr::Address::kMtval:                                                    return;//We simply ignore writes to MTVAL, NOT throw an exception
        case Csr::Address::kMip:              this->mip       = data & 0b00000000000000000000'0010'0010'0010; return;//Note ALL interrupt pending bits for M-mode are READ ONLY

        //FIXME when locked, ignore (not throw exception) on writes to the relevant PMP CSRs
        case Csr::Address::kPmpcfgStart  ... Csr::Address::kPmpcfgEnd:    this->pmpcfg [static_cast<uint16_t>(csr) - static_cast<uint16_t>(Csr::Address::kPmpcfgStart)] = data; return;//FIXME WARL
        case Csr::Address::kPmpaddrStart ... Csr::Address::kPmpaddrEnd:   this->pmpaddr[static_cast<uint16_t>(csr) - static_cast<uint16_t>(Csr::Address::kPmpaddrStart)] = data; return;//FIXME WARL

        case Csr::Address::kMcycle:           this->mcycle    = (this->mcycle   & 0xFFFFFFFF00000000) | ((uint64_t) data.u); return;
        case Csr::Address::kMinstret:         this->minstret  = (this->minstret & 0xFFFFFFFF00000000) | ((uint64_t) data.u); return;

        case Csr::Address::kMhpmcounterStart ... Csr::Address::kMhpmcounterEnd: return;//We simply ignore writes to the HPMCOUNTER CSRs, NOT throw exceptions

        case Csr::Address::kMcycleh:          this->mcycle    = (this->mcycle   & 0x00000000FFFFFFFF) | (((uint64_t) data.u) << 32); return;
        case Csr::Address::kMinstreth:        this->minstret  = (this->minstret & 0x00000000FFFFFFFF) | (((uint64_t) data.u) << 32); return;

        case Csr::Address::kMhpmcounterhStart ... Csr::Address::kMhpmcounterhEnd: return;//We simply ignore writes to the HPMCOUNTERH CSRs, NOT throw exceptions

        case Csr::Address::kMtime:            this->mtime     = (this->mtime    & 0xFFFFFFFF00000000) | ((uint64_t)  data.u);         return;//Custom
        case Csr::Address::kMtimeh:           this->mtime     = (this->mtime    & 0x00000000FFFFFFFF) | (((uint64_t) data.u) << 32);  return;//Custom
        case Csr::Address::kMtimecmp://Custom
            this->mtimecmp  = (this->mtimecmp & 0xFFFFFFFF00000000) | ((uint64_t)  data.u);
            this->mip &= ~(1 << 7);//Clear mip.MTIP on writes to mtimecmp (which would normally be in memory, but we made it a CSR so might as well handle it here)
            return;
        case Csr::Address::kMtimecmph://Custom
            this->mtimecmp  = (this->mtimecmp & 0x00000000FFFFFFFF) | (((uint64_t) data.u) << 32);
            this->mip &= ~(1 << 7);//Clear mip.MTIP on writes to mtimecmp (which would normally be in memory, but we made it a CSR so might as well handle it here)
            return;

        default:                        invoke_rv_exception(ILLEGAL_INSTRUCTION);
    }
}

void Csr::set_privilege_mode(PrivilegeMode new_privilege_mode) {
    this->m_privilege_mode = new_privilege_mode;
}

PrivilegeMode Csr::get_privilege_mode() const {
    return this->m_privilege_mode;
}

void Csr::update_timer() {
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
