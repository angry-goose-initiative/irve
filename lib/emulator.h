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
#include "memory.h"
#include "decode.h"

#include "rvexception.h"

/* Types */

namespace irve::internal::emulator {
    class emulator_t {
    public:

        // TODO how to document this?
        /**
         * The default constructor is deleted since the emulator is useless without a memory image
         * file loaded
        */
        emulator_t() = delete;

        /**
         * @brief The constructor
         * @param imagev TODO
        */
        emulator_t(int imagec, char** imagev);

        /**
         * @brief TODO
         * @return True as long as the emulator should continue running
        */
        bool tick();
        
        /**
         * @brief Runs the emulator until the given instruction count is reached or an exit request is made.
         *        For dynamic linking to libirve, this is more efficient than calling tick() in a loop.
         * @param inst_count TODO
        */
        void run_until(uint64_t inst_count);

        /**
         * @brief TODO
         * @return TODO
        */
        uint64_t get_inst_count() const;

    private:

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
        void handle_interrupt(rvexception::cause_t cause);

        /**
         * @brief TODO
         * @param cause TODO
        */
        void handle_exception(rvexception::cause_t cause);
        
        CSR::CSR_t m_CSR;
        memory::memory_t m_memory;
        cpu_state::cpu_state_t m_cpu_state;

        //TODO other things
    };
}

#endif//EMULATOR_H
