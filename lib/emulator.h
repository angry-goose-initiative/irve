/**
 * @brief   The main emulator class used to instantiate an instance of IRVE.
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

#include <cstdint>

#include "common.h"
#include "cpu_state.h"
#include "decode.h"
#include "gdbserver.h"
#include "memory.h"
#include "rvexception.h"
#include "semihosting.h"

#include <unordered_map>

/* ------------------------------------------------------------------------------------------------
 * Type/Class Declarations
 * --------------------------------------------------------------------------------------------- */

/**
 * @brief       The namespace containing the actual emulator_t class (internal).
*/
namespace irve::internal::emulator {
    /**
     * @brief       The main IRVE emulator class (internal).
    */
    class emulator_t {
    public:
        /**
         * The default constructor is deleted since the emulator is useless without a memory image
         * file loaded.
        */
        emulator_t() = delete;

        /**
         * @brief       The constructor for emulator_t.
         * @param[in]   imagec The number of memory image files to load.
         * @param[in]   imagev Vector of memory image file names.
        */
        emulator_t(int imagec, const char* const* imagev);

        /**
         * @brief       Emulate one instruction.
         * @return      True if the emulator should continue running, false otherwise.
        */
        bool tick();
         
        /**
         * @brief       Repeatedly emulate instructions.
         * @details     Runs the emulator until the given instruction count is reached or an exit
         *              request is made. For dynamic linking to libirve, this is more efficient
         *              than calling tick() in a loop.
         * @param[in]   inst_count The value of minstret at which to stop.
        */
        void run_until(uint64_t inst_count);

        /**
         * @brief       Run a GDB server on the given port.
         * @param[in]   port The port to listen on.
        */
        void run_gdbserver(uint16_t port);

        /**
         * @brief       Get the current instruction count.
         * @return      minstret
        */
        uint64_t get_inst_count() const;

        /**
         * @brief       Determine if a breakpoint was encountered and clear the flag indicating so
         *              if it was.
         * @return      True if a breakpoint was encountered.
        */
        bool test_and_clear_breakpoint_encountered_flag();

        /**
         * @brief       Flish the instruction cache.
        */
        void flush_icache();

    private:

        /**
         * @brief       Fetches and decodes the instruciton specified by the current PC.
         * @return      Information about the decoded instruciton.
        */
        decode::decoded_inst_t fetch_and_decode();

        /**
         * @brief       Executes an instruction that has been decoded.
         * @param[in]   decoded_inst Information about the decoded instruction.
        */
        void execute(const decode::decoded_inst_t& decoded_inst);//TODO move this to a separate file

        /**
         * @brief       Check for interrupts and if any have occurred, handle them.
        */
        void check_and_handle_interrupts();

        /**
         * @brief       Handle a trap.
         * @param[in]   cause The cause of the trap.
        */
        void handle_trap(rvexception::cause_t cause);
        
        Csr m_CSR;
        Memory m_memory;
        CpuState m_cpu_state;

        semihosting::handler_t m_semihosting_handler;
        std::unordered_map<uint32_t, decode::decoded_inst_t> m_icache;//uint32_t to avoid needing to implement hash for Word
        bool m_intercept_breakpoints;
        bool m_encountered_breakpoint;
    };
}
