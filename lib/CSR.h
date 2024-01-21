/**
 * @brief   Contains (mostly) CSRs for IRVE as well as methods to read and write them.
 * 
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  Copyright (C) 2023 Nick Chan\n
 *  See the LICENSE file at the root of the project for licensing info.
*/

#pragma once

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#ifdef private//Unit tests define this but this dosn't play nicely with chrono
#undef private
#include <chrono>
#define private public
#else
#include <chrono>
#endif

#include <cstdint>

#include "common.h"
#include "rvexception.h"

namespace irve::internal {

/**
 * @brief       RISC-V privilege modes.
 * @note        0b10 is reserved.
*/
enum class PrivilegeMode : uint8_t {
    USER_MODE       = 0b00,
    SUPERVISOR_MODE = 0b01,
    MACHINE_MODE    = 0b11
};

/**
 * @brief       Class containing RISC-V CSR's.
*/
class Csr {
public:

    /**
     * @brief       The addresses of the CSR's that are implemented by IRVE.
    */
    enum class Address : uint16_t {
        kSstatus            = 0x100,
        kSie                = 0x104,
        kStvec              = 0x105,
        kScounteren         = 0x106,
        kSenvcfg            = 0x10A,
        kSscratch           = 0x140,
        kSepc               = 0x141,
        kScause             = 0x142,
        kStval              = 0x143,
        kSip                = 0x144,
        kSatp               = 0x180,
        kMstatus            = 0x300,
        kMisa               = 0x301,
        kMedeleg            = 0x302,
        kMideleg            = 0x303,
        kMie                = 0x304,
        kMtvec              = 0x305,
        kMcounteren         = 0x306,
        kMenvcfg            = 0x30A,
        kMstatush           = 0x310,
        kMenvcfgh           = 0x31A,
        kMcountinhibit      = 0x320,

        kMhpmeventStart      = 0x323, // Inclusive
        kMhpmeventEnd        = 0x33F, // Inclusive

        kMscratch           = 0x340,
        kMepc               = 0x341,
        kMcause             = 0x342,
        kMtval              = 0x343,
        kMip                = 0x344,
        kMinst              = 0x34A,
        kMtval2             = 0x34B,

        kPmpcfgStart        = 0x3A0, // Inclusive
        kPmpcfgEnd          = 0x3AF, // Inclusive
        kPmpaddrStart       = 0x3B0, // Inclusive
        kPmpaddrEnd         = 0x3EF, // Inclusive

        kMcycle             = 0xB00,
        kMinstret           = 0xB02,

        kMhpmcounterStart   = 0xB03, // Inclusive
        kMhpmcounterEnd     = 0xB1F, // Inclusive

        kMcycleh            = 0xB80,
        kMinstreth          = 0xB82,

        kMhpmcounterhStart  = 0xB83,
        kMhpmcounterhEnd    = 0xB9F,

        kMtime              = 0xBC0, // Custom
        kMtimeh             = 0xBC4, // Custom
        kMtimecmp           = 0xBD0, // Custom
        kMtimecmph          = 0xBD4, // Custom

        kCycle              = 0xC00,
        kTime               = 0xC01,
        kInstret            = 0xC02,

        kHpmcounterStart    = 0xC03, // Inclusive
        kHpmcounterEnd      = 0xC1F, // Inclusive

        kCycleh             = 0xC80,
        kTimeh              = 0xC81,
        kInstreth           = 0xC82,

        kHpmcounterhStart   = 0xC83, // Inclusive
        kHpmcounterhEnd     = 0xC9F, // Inclusive

        kMvendorid          = 0xF11,
        kMarchid            = 0xF12,
        kMimpid             = 0xF13,
        kMhartid            = 0xF14,
        kMconfigptr         = 0xF15,
    };

    /**
     * @brief       The default Csr constructor. 
     * @note        Only guaranteed to initialize CSRs that must be according to the RISC-V spec.
    */
    Csr();

    /**
     * @brief       Reads a CSR explicitly (checks for adequate privilege and readablity).
     * @note        Invokes an illegal instruction exception if the CSR cannot be explicitly read
     *              from.
     * @param[in]   csr The CSR to read from.
     * @return      The value of the CSR.
    */
    Reg explicit_read(Csr::Address csr);//Not constant due to possible read side effects

    /**
     * @brief       Writes a CSR explicitly (checking for adequate privilege and writability).
     * @note        Invokes an illegal instruction exception if the CSR cannot be explicitly
     *              written to.
     * @param[in]   csr The CSR to write to.
     * @param[in]   data The data to write to the CSR.
    */
    void explicit_write(Csr::Address csr, Word data);

    /**
     * @brief       Reads a CSR implicitly (without checking privilege; still checks readablity).
     * @note        If the CSR number is invalid, an illegal instruction exception is invoked.
     *              Implicit CSR accesses should never be invalid; this exception is invoked since
     *              explicit CSR accesses do not check for an invalid CSR number before calling
     *              this function.
     * @param[in]   csr The CSR to read from.
     * @return      The value of the CSR.
    */
    Reg implicit_read(Csr::Address csr);//Not constant due to possible read side effects

    /**
     * @brief       Writes a CSR implicitly (without checking privilege; still checks writability).
     * @note        If the CSR number is invalid, an illegal instruction exception is invoked.
     *              Implicit CSR accesses should never be invalid; this exception is invoked since
     *              explicit CSR accesses do not check for an invalid CSR number before calling
     *              this function.
     * @param[in]   csr The CSR to write to.
     * @param[in]   data The data to write to the CSR.
    */
    void implicit_write(Csr::Address csr, Word data);//Does not perform privilege checks

    /**
     * @brief       Sets the privilege mode of the RISC-V CPU.
     * @param[in]   new_privilege_mode The new privilege mode to use.
    */
    void set_privilege_mode(PrivilegeMode new_privilege_mode);

    /**
     * @brief       Gets the privilege mode of the RISC-V CPU.
     * @return      The current privilege mode.
    */
    PrivilegeMode get_privilege_mode() const;

    /**
     * @brief       Updates the RISC-V CPU's mtime timer based on the host system's time.
     *              May also set a timer interrupt as pending in the mip CSR.
    */
    void update_timer();

    /**
     * @brief       Occasionally calls update_timer(), but not necessarily on every call since
     *              this may be too expensive.
    */
    void occasional_update_timer();

private:

    /**
     * @brief       Checks if the current privilege mode can read a CSR.
     * @param[in]   csr The CSR to check.
     * @return      True if the current privilege mode can read the CSR, false otherwise.
    */
    bool current_privilege_mode_can_explicitly_read(Csr::Address csr) const;

    /**
     * @brief       Checks if the current privilege mode can write a CSR.
     * @param[in]   csr The CSR to check.
     * @return      True if the current privilege mode can write the CSR, false otherwise.
    */
    bool current_privilege_mode_can_explicitly_write(Csr::Address csr) const;

    Reg sie;
    Reg stvec;
    Reg scounteren;
    Reg senvcfg;
    Reg sscratch;
    Reg sepc;
    Reg scause;
    //stval is NOT here
    Reg sip;
    Reg satp;
    Reg mstatus;
    //misa is NOT here
    Reg medeleg;
    Reg mideleg;
    Reg mie;
    Reg mtvec;
    Reg menvcfg;
    //mstatush is NOT here
    //menvcfgh is NOT here
    Reg mscratch;
    Reg mepc;
    Reg mcause;
    //mtval is NOT here
    Reg mip;

    Reg pmpcfg[64];
    Reg pmpaddr[64];

    uint64_t minstret;//Handles both minstret and minstreth
    uint64_t mcycle;//Handles both mcycle and mcycleh

    //NOTE: According to the spec, mtime and mtimecmp must be in memory, not in CSR's. However,
    //      that would mean Csr would need a reference to memory, which is not ideal. Instead we
    //      keep them here, and memory will have to redirect writes to their addresses into
    //      implicit writes to these CSR's.

    uint64_t mtime;//Handles both time and timeh
    uint64_t mtimecmp;//Handles both time and timeh
    std::chrono::time_point<std::chrono::steady_clock> m_last_time_update;
    uint16_t m_delay_update_counter;//Don't check how much time has passed each tick() (much too slow)

    /**
     * @brief       The current privilege mode of the hart.
     * @note        This is not a CSR according to the spec, but it is used to determine if CSR's
     *              can be accessed. It also makes sense for this to be stored here since the
     *              current privilege mode logically fits under the umbrella of a "control status
     *              register".
    */
    PrivilegeMode m_privilege_mode;
};

} // namespace irve::internal
