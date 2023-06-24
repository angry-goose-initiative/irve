/* emulator.h
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * The main emulator class used to instantiate an instance of irve
 *
*/

#ifndef EMULATOR_H
#define EMULATOR_H

/* Includes */

#include "common.h"
#include "cpu_state.h"
#include "decode.h"
#include "gdbserver.h"
#include "memory.h"
#include "rvexception.h"

/* Types */

/**
 * @brief The namespace containing the actual emulator_t class (internal)
*/
namespace irve::internal::emulator {
    /**
     * @brief The main IRVE emulator class (internal)
    */
    class emulator_t {
    public:
        /**
         * The default constructor is deleted since the emulator is useless without a memory image
         * file loaded
        */
        emulator_t() = delete;

        /**
         * @brief The constructor
         * @param imagev TODO
        */
        emulator_t(int imagec, const char* const* imagev);

        /**
         * @brief Emulate one instruction
        */
        bool tick();//Returns true if the emulator should continue running
         
        /**
         * @brief Repeatedly emulate instructions
         * @param inst_count The value of minstret at which to stop
         * Runs the emulator until the given instruction count is reached or an exit request is made
         * For dynamic linking to libirve, this is more efficient than calling tick() in a loop
         *
        */
        void run_until(uint64_t inst_count);

        /**
         * @brief Run a GDB server on the given port
         * @param port The port to listen on
         *
        */
        void run_gdbserver(uint16_t port);

        /**
         * @brief Get the current instruction count
         * @return minstret
        */
        uint64_t get_inst_count() const;

        /**
         * @brief Determine if a breakpoint was encountered and clear the flag indicating so if it was
         * @return True if a breakpoint was encountered
        */
        bool test_and_clear_breakpoint_encountered_flag();
    private:
        
        //TODO document these as well

        /**
         * @brief TODO
         * @return TODO
        */
        word_t fetch();

        /**
         * @brief TODO
         * @param decoded_inst TODO
        */
        void execute(const decode::decoded_inst_t& decoded_inst);//TODO move this to a separate file

        /**
         * @brief TODO
         * @param cause TODO
        */
        void check_and_handle_interrupts();

        /**
         * @brief TODO
         * @param cause TODO
        */
        void handle_exception(rvexception::cause_t cause);
        
        CSR::CSR_t m_CSR;
        memory::memory_t m_memory;
        cpu_state::cpu_state_t m_cpu_state;
        bool m_encountered_breakpoint;

        //TODO other things
    };
}

#endif//EMULATOR_H
