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

//TODO move these to the address namespace

#define MEPC_ADDRESS 0x341

//TODO fill in the x's with supported extensions
#define MISA_CONTENTS word_t(0b010000xxxxxxxxxxxxxxxxxxxxxxxxxx)

//MTVEC hardcoded to start at 0x00000004 and be vectored
#define MTVEC_CONTENTS word_t((0x00000004 << 2) | 0b01)

namespace irve::internal::CSR {

    /* Constants */

    namespace address {
        //TODO list of CSR addresses here
        const word_t SSCRATCH       = 0x140;
        const word_t SEPC           = 0x141;
        const word_t SCAUSE         = 0x142;
        const word_t STVAL          = 0x143;
        const word_t SIP            = 0x144;
        //const word_t SATP           = 0x180;
        const word_t MSTATUS        = 0x300;
        const word_t MISA           = 0x301;
        const word_t MEDELEG        = 0x302;
        const word_t MIDELEG        = 0x303;
        const word_t MIE            = 0x304;
        const word_t MTVEC          = 0x305;
        const word_t MCOUNTEREN     = 0x306;
        const word_t MENVCFG        = 0x30A;
        const word_t MSTATUSH       = 0x310;
        const word_t MENVCFGH       = 0x31A;
        const word_t MCOUNTINHIBIT  = 0x320;
        //TODO the event counters
        const word_t MSCRATCH       = 0x340;
        const word_t MEPC           = 0x341;
        const word_t MCAUSE         = 0x342;
        const word_t MTVAL          = 0x343;
        const word_t MIP            = 0x344;
        const word_t MTINST         = 0x34A;
        const word_t MTVAL2         = 0x34B;
        //TODO the PMP CSRs
        //const word_t SATP           = 0x5A8;
        const word_t MSECCFG        = 0x747;
        const word_t MSECCFGH       = 0x757;
        const word_t MCYCLE         = 0xB00;
        const word_t MINSTRET       = 0xB02;
        //TODO the event counters
        const word_t MCYCLEH        = 0xB80;
        const word_t MINSTRETH      = 0xB82;
        //TODO the event counters
        const word_t CYCLE          = 0xC00;
        const word_t TIME           = 0xC01;
        const word_t INSTRET        = 0xC02;
        //TODO the event counters
        const word_t CYCLEH         = 0xC80;
        const word_t TIMEH          = 0xC81;
        const word_t INSTRETH       = 0xC82;
        //TODO the event counters
        const word_t MVENDORID      = 0xF11;
        const word_t MARCHID        = 0xF12;
        const word_t MIMPID         = 0xF13;
        const word_t MHARTID        = 0xF14;
        const word_t MCONFIGPTR     = 0xF15;

    };

    /* Types */

    enum class privilege_mode_t : uint8_t {
        USER_MODE = 0b00,
        SUPERVISOR_MODE = 0b01,
        MACHINE_MODE = 0b11
    };

    /* Function/Class Declarations */

    class CSR_t {
    public:
        CSR_t();

        //Use these to implement CSR instructions
        reg_t explicit_read(uint16_t csr) const;
        void explicit_write(uint16_t csr, word_t data);
        reg_t implicit_read(uint16_t csr) const;
        void implicit_write(uint16_t csr, word_t data);

        void set_privilege_mode(privilege_mode_t new_privilege_mode);
        privilege_mode_t get_privilege_mode() const;

        //TODO add way to implicitly read/write CSRs so they won't cause exceptions (ex. for timers, etc.)

        void increment_inst_count();
        uint64_t get_inst_count() const;


        //TODO perhaps leave these public to use them for implicit reads/writes?

        //TODO make these private

        reg_t sscratch;//Address 0x140
        reg_t sepc;//Address 0x141
        union {//Address 0x142
            rvexception::cause_t as_cause_t;
            reg_t as_reg_t;
        } scause;
        reg_t stval;//Address 0x143
        //TODO sip here or somewhere else?//Address 0x144
        //TODO satp here or somewhere else?//Address 0x180

        //TODO mstatus here or somewhere else?//Address 0x300
        //misa is NOT here//Address 0x301
        reg_t medeleg;//Address 0x302
        reg_t mideleg;//Address 0x303
        //TODO mie here or somewhere else?//Address 0x304
        //TODO mtvec here or somewhere else?//Address 0x305
        //TODO mcounteren here or somewhere else?//Address 0x306
        //TODO menvcfg here or somewhere else?//Address 0x30A
        //TODO mstatush here or somewhere else?//Address 0x310

        reg_t mepc;//Address 0x341
        union {
            rvexception::cause_t as_cause_t;//Address 0x342
            reg_t as_reg_t;
        } mcause;

        //TODO add CSRs HERE

    private:
        privilege_mode_t m_privilege_mode;//Not a CSR, but it is a register we need to access to determine if we can access a CSR (and it is also used in other places)
        uint64_t minstret;
    };
}

#endif
