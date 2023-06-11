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
         * @brief Construct a new emulator_t
        */
        emulator_t();

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
         * @brief Get the current instruction count
         * @return minstret
        */
        uint64_t get_inst_count() const;

        /**
         * @brief Read a byte from memory
         * @param addr The address to read from
         * @return The byte at the given address
        */
        int8_t mem_read_byte(word_t addr) const;

        /**
         * @brief Write a byte to memory
         * @param addr The address to write to
         * @param data The byte to write
        */
        void mem_write(word_t addr, uint8_t size, word_t data);

    private:
        
        //TODO document these as well

        word_t fetch() const;
        void execute(const decode::decoded_inst_t& decoded_inst);//TODO move this to a separate file

        void handle_interrupt(rvexception::cause_t cause);
        void handle_exception(rvexception::cause_t cause);
        
        CSR::CSR_t m_CSR;
        memory::memory_t m_memory;
        cpu_state::cpu_state_t m_cpu_state;

        //TODO other things
    };
}

#endif//EMULATOR_H
