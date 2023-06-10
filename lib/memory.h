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

//TODO put these into a namespace as regular C++ constants

#define RAMSIZE 0x04000000//64MB (should be plenty to begin with)
#define SECOND_RAMSIZE 0x04000000//Starting at 0xC0000000//64MB (should be plenty for the kernel to begin with)
#define DEBUGADDR 0xFFFFFFFF//RISC-V code that writes a series of bytes to this address will print them to stdout (flushed when a newline is encountered)

/* Includes */

#include <iostream>
#include <memory>

#include "common.h"
#include "CSR.h"

/* Function/Class Declarations */

namespace irve::internal::memory {

    class pmemory_t {//Physical memory
    public:
        pmemory_t();
        ~pmemory_t();

        //TODO if/when we implement PMP, this will need an m_CSR_ref too

        uint8_t r(word_t addr) const;//TODO this can't be const because we need to handle read side effects
        void w(word_t addr, uint8_t data);
    private:
        std::unique_ptr<uint8_t[]> m_ram;
        std::unique_ptr<uint8_t[]> m_second_ram;
        std::string m_debugstr;
    };

    class memory_t {//Virtual memory (or passthru for physical memory if virtual memory is disabled)
    private:
        // The "physical" memory
        pmemory_t m_mem;

        CSR::CSR_t& m_CSR_ref;
    public:
        memory_t(CSR::CSR_t& CSR_ref);
        word_t r(word_t addr, int8_t func3) const;
        void w(word_t addr, int8_t func3, word_t data);
        void p(word_t addr) const;

        //TODO functions to deal with virtual memory and also to disable it when the emulated cpu is in Machine mode
    };

}

#endif
