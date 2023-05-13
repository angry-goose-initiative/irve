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

using namespace irve::internal;

/* Types */

namespace irve::internal::emulator {
    class emulator_t {//TODO provide read-only access to the CPU state at the end for integration testing
    public:
        emulator_t();

        bool tick();//Returns true as long as the emulator should continue running
         
        //Runs the emulator until the given instruction count is reached or an exit request is made
        //For dynamic linking to libirve, this is more efficient than calling tick() in a loop
        void run_until(uint64_t inst_count);

        uint64_t get_inst_count() const;

        int8_t mem_read_byte(word_t addr) const;
        void mem_write(word_t addr, uint8_t size, word_t data);

    private:
        word_t fetch() const;
        void execute(const decoded_inst_t &decoded_inst);//TODO move this to a separate file
        
        memory_t m_memory;
        cpu_state_t m_cpu_state;

        //TODO other things
    };
}

#endif//EMULATOR_H
