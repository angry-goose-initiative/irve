/* memory.hpp
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * A class for the memory (RAM, not MMIO) of the emulator
 * 
*/

#ifndef MEMORY_H
#define MEMORY_H

/* Includes */

#include <iostream>

#define MEMSIZE 1024

class memory_t {
private:
    // The "physical" memory
    int8_t m_mem[MEMSIZE];
public:
    memory_t();
    int32_t r(const uint32_t addr, const int8_t func3) const;
    void w(const uint32_t addr, const int8_t func3, const int32_t data);
    void p(const uint32_t addr) const;
};

#endif
