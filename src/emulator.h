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

#include "cpu_state.h"
#include "memory.h"
#include "decode.h"

/* Types */

class emulator_t {
public:
    emulator_t();

    void tick();

    uint64_t get_inst_count() const;

    int8_t mem_read_byte(uint32_t addr) const;
    void mem_write(uint32_t addr, uint8_t size, int32_t data);

private:
    word_t fetch() const;
    void execute(const decoded_inst_t &decoded_inst);//TODO move this to a separate file

    memory_t m_memory;
    cpu_state_t m_cpu_state;

    //TODO other things
};

#endif//EMULATOR_H
