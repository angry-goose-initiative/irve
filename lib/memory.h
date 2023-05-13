/* memory.hpp
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * A class for the memory of the emulator
 *
 * Manages virtual memory and physical memory
 * 
*/

#ifndef MEMORY_H
#define MEMORY_H

/* Constants And Defines */

#define RAMSIZE 0x04000000//64MB (should be plenty for the kernel to begin with)
#define DEBUGADDR 0xFFFFFFFF//RISC-V code that writes a series of bytes to this address will print them to stdout (flushed when a newline is encountered)

/* Includes */

#include <iostream>

#include "common.h"

/* Function/Class Declarations */

//TODO namespacing

class pmemory_t {//Physical memory
public:
    pmemory_t();
    ~pmemory_t();

    uint8_t r(irve::internal::word_t addr) const;//TODO this can't be const because we need to handle read side effects
    void w(irve::internal::word_t addr, uint8_t data);
private:
    uint8_t* m_ram;
    std::string m_debugstr;
};

//TODO namespacing

class memory_t {//Virtual memory (or passthru for physical memory if virtual memory is disabled)
private:
    // The "physical" memory
    pmemory_t m_mem;
    //int8_t m_mem[PMEMSIZE];
public:
    memory_t();
    irve::internal::word_t r(irve::internal::word_t addr, int8_t func3) const;
    void w(irve::internal::word_t addr, int8_t func3, irve::internal::word_t data);
    void p(irve::internal::word_t addr) const;

    //TODO functions to deal with virtual memory and also to disable it when the emulated cpu is in Machine mode
};

#endif
