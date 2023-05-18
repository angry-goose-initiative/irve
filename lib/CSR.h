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

//                                   ABCDEFGHIJKLMNOPQRSTUVWXYZ
#define MISA_CONTENTS word_t(0b01000010000000100010000010100100)

//MTVEC hardcoded to start at 0x00000004 and be vectored
#define MTVEC_CONTENTS word_t((0x00000004 << 2) | 0b01)

namespace irve::internal::CSR {

    /* Constants */

    namespace address {
        //TODO list of CSR addresses here
        const uint16_t SSCRATCH       = 0x140;
        const uint16_t SEPC           = 0x141;
        const uint16_t SCAUSE         = 0x142;
        const uint16_t STVAL          = 0x143;
        const uint16_t SIP            = 0x144;
        //const uint16_t SATP           = 0x180;
        const uint16_t MSTATUS        = 0x300;
        const uint16_t MISA           = 0x301;
        const uint16_t MEDELEG        = 0x302;
        const uint16_t MIDELEG        = 0x303;
        const uint16_t MIE            = 0x304;
        const uint16_t MTVEC          = 0x305;
        const uint16_t MCOUNTEREN     = 0x306;
        const uint16_t MENVCFG        = 0x30A;
        const uint16_t MSTATUSH       = 0x310;
        const uint16_t MENVCFGH       = 0x31A;
        const uint16_t MCOUNTINHIBIT  = 0x320;
        //TODO the event counters
        const uint16_t MSCRATCH       = 0x340;
        const uint16_t MEPC           = 0x341;
        const uint16_t MCAUSE         = 0x342;
        const uint16_t MTVAL          = 0x343;
        const uint16_t MIP            = 0x344;
        const uint16_t MTINST         = 0x34A;
        const uint16_t MTVAL2         = 0x34B;
        //TODO the PMP CSRs
        //const uint16_t SATP           = 0x5A8;
        const uint16_t MCYCLE         = 0xB00;
        const uint16_t MINSTRET       = 0xB02;
        //TODO the event counters
        const uint16_t MCYCLEH        = 0xB80;
        const uint16_t MINSTRETH      = 0xB82;
        //TODO the event counters
        const uint16_t CYCLE          = 0xC00;
        const uint16_t TIME           = 0xC01;
        const uint16_t INSTRET        = 0xC02;
        //TODO the event counters
        const uint16_t CYCLEH         = 0xC80;
        const uint16_t TIMEH          = 0xC81;
        const uint16_t INSTRETH       = 0xC82;
        //TODO the event counters
        const uint16_t MVENDORID      = 0xF11;
        const uint16_t MARCHID        = 0xF12;
        const uint16_t MIMPID         = 0xF13;
        const uint16_t MHARTID        = 0xF14;
        const uint16_t MCONFIGPTR     = 0xF15;

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

    private:
        bool valid_explicit_read_at_current_privilege_mode(uint16_t csr) const;
        bool valid_explicit_write_at_current_privilege_mode(uint16_t csr) const;

        reg_t sscratch;
        reg_t sepc;
        reg_t scause;
        reg_t stval;
        reg_t sip;
        //reg_t satp;
        reg_t mstatus;
        //misa is NOT here
        reg_t medeleg;
        reg_t mideleg;
        reg_t mie;
        reg_t mtvec;
        reg_t mcounteren;
        reg_t menvcfg;
        reg_t mstatush;
        reg_t menvcfgh;
        reg_t mcountinhibit;
        //TODO the event counters
        reg_t mscratch;
        reg_t mepc;
        reg_t mcause;
        reg_t mtval;
        //mtval is NOT here
        reg_t mip;
        reg_t mtinst;
        reg_t mtval2;
        //TODO the PMP CSRs
        //reg_t satp;
        //TODO the event counters

        //TODO add CSRs HERE

        uint64_t minstret;//Handles both minstret and minstreth
        uint64_t mcycle;//Handles both mcycle and mcycleh
        uint64_t cycle;//Handles both cycle and cycleh
        uint64_t time;//Handles both time and timeh
        uint64_t instret;//Handles both instret and instreth

        privilege_mode_t m_privilege_mode;//Not a CSR, but it is a register we need to access to determine if we can access a CSR (and it is also used in other places)
    };
}

#endif
