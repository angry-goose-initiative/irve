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

/* ------------------------------------------------------------------------------------------------
 * Type/Class Declarations
 * --------------------------------------------------------------------------------------------- */

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

    // The addresses of the CSR's that are implemented by IRVE.
    enum class Address : uint16_t {
        SSTATUS              = 0x100,
        SIE                  = 0x104,
        STVEC                = 0x105,
        SCOUNTEREN           = 0x106,
        SENVCFG              = 0x10A,
        SSCRATCH             = 0x140,
        SEPC                 = 0x141,
        SCAUSE               = 0x142,
        STVAL                = 0x143,
        SIP                  = 0x144,
        SATP                 = 0x180,
        MSTATUS              = 0x300,
        MISA                 = 0x301,
        MEDELEG              = 0x302,
        MIDELEG              = 0x303,
        MIE                  = 0x304,
        MTVEC                = 0x305,
        MCOUNTEREN           = 0x306,
        MENVCFG              = 0x30A,
        MSTATUSH             = 0x310,
        MENVCFGH             = 0x31A,
        MCOUNTINHIBIT        = 0x320,

        MHPMEVENT_START      = 0x323, // Inclusive
        MHPMEVENT_END        = 0x33F, // Inclusive

        MSCRATCH             = 0x340,
        MEPC                 = 0x341,
        MCAUSE               = 0x342,
        MTVAL                = 0x343,
        MIP                  = 0x344,
        MTINST               = 0x34A,
        MTVAL2               = 0x34B,
        
        PMPCFG_START         = 0x3A0, // Inclusive
        PMPCFG_END           = 0x3AF, // Inclusive
        PMPADDR_START        = 0x3B0, // Inclusive
        PMPADDR_END          = 0x3EF, // Inclusive
        
        MCYCLE               = 0xB00,
        MINSTRET             = 0xB02,

        MHPMCOUNTER_START    = 0xB03, // Inclusive
        MHPMCOUNTER_END      = 0xB1F, // Inclusive

        MCYCLEH              = 0xB80,
        MINSTRETH            = 0xB82,

        MHPMCOUNTERH_START   = 0xB83, // Inclusive
        MHPMCOUNTERH_END     = 0xB9F, // Inclusive

        MTIME                = 0xBC0, // Custom
        MTIMEH               = 0xBC4, // Custom
        MTIMECMP             = 0xBD0, // Custom
        MTIMECMPH            = 0xBD4, // Custom

        CYCLE                = 0xC00,
        TIME                 = 0xC01,
        INSTRET              = 0xC02,

        HPMCOUNTER_START     = 0xC03, // Inclusive
        HPMCOUNTER_END       = 0xC1F, // Inclusive

        CYCLEH               = 0xC80,
        TIMEH                = 0xC81,
        INSTRETH             = 0xC82,

        HPMCOUNTERH_START    = 0xC83, // Inclusive
        HPMCOUNTERH_END      = 0xC9F, // Inclusive

        MVENDORID            = 0xF11,
        MARCHID              = 0xF12,
        MIMPID               = 0xF13,
        MHARTID              = 0xF14,
        MCONFIGPTR           = 0xF15,
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

    Reg stvec;
    Reg scounteren;
    Reg senvcfg;
    Reg sscratch;
    Reg sepc;
    Reg scause;
    Reg stval;
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
