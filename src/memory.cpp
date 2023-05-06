/* memory.cpp
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 * 
 * A class for the memory (RAM, not MMIO) of the emulator
 *
 * TODO change this class to deal with virtual memory when the CPU is in user mode and
 * to actually in fact deal with MMIO too
 * 
*/

/* Includes */

#include "memory.h"

#include <cassert>
#include <cmath>
#include <iostream>

#include "common.h"

#define INST_COUNT 0//We only log at init
#include "logging.h"

/* Function Implementations */

//Virtual memory

// All of memory is initialized to 0
memory_t::memory_t(): m_mem() {
    irvelog(1, "Created new Memory instance");
}

// Read from memory
int32_t memory_t::r(word_t addr, int8_t func3) const {


    // inaccessable address exceptions:
    //  - vacant addresses are never accessible

    // should memory accesses be word-aligned? (if misaligned is allowed caching will be harder)

    // refuse to run RVTSO binaries?

    assert((func3 >= 0b000) && (func3 <= 0b111) && "Invalid func3");
    //TODO from now on exceptions for invalid physical memory accesses will be thrown by pmemory_t instead
    //TODO we will have to handle page faults here though
    //assert((addr < MEMSIZE) && "Invalid memory address");  // TODO throw exceptions to be caught?
    //assert(((addr + pow(2, func3%4) - 1) < MEMSIZE) && "Invalid memory address");

    // MSB of func3 determines signed/unsigned
    const int isUnsigned{func3 >> 2};
    // 2^(func3[1:0]) is the number of bytes
    int byte{static_cast<int>(pow(2, func3%4)) - 1};

    int32_t data{};
    data = static_cast<int32_t>(m_mem.r(addr + byte)) << (8*byte);

    // The casting above sign extends so if the number is unsigned then we need
    // to remove the sign extention
    if(isUnsigned) data &= (0xFF << (8*byte));
    --byte;
    
    for(; byte > -1; --byte) {
        data |= (static_cast<int32_t>(m_mem.r(addr + byte)) & 0xFF) << (8*byte);
    }

    return data;
}

// Write to memory
void memory_t::w(word_t addr, int8_t func3, word_t data) {

    assert((func3 >= 0b000) && (func3 <= 0b010) && "Invalid func3");
    //TODO from now on exceptions for invalid physical memory accesses will be thrown by pmemory_t instead
    //TODO we will have to handle page faults here though
    //assert((addr < MEMSIZE) && "Invalid memory address"); // TODO throw exceptions to be caught?
    //assert(((addr + pow(2, func3%4) - 1) < MEMSIZE) && "Invalid memory address");

    // 2^(func3[1:0]) is the number of bytes
    int byte{static_cast<int>(pow(2, func3%4))};

    for(int i{}; i<byte; ++i) {
        m_mem.w(addr + i, static_cast<int8_t>((data.srl(8 * i) & 0xFF).u));
    }
}

// TODO integrate with logging or delete
// Prints the 8 bytes at and following the specified address in hex
void memory_t::p(const uint32_t addr) const {

    for(int byte{}; byte<8; ++byte) {
        // top 4 bits
        std::cout << std::hex << ((m_mem.r(addr + byte) >> 4) & 0xF);
        // bottom 4 bits
        std::cout << std::hex << (m_mem.r(addr + byte) & 0xF);
        std::cout << " ";
    }
    std::cout << std::endl;
}

//Physical memory

pmemory_t::pmemory_t(): m_ram(new uint8_t[RAMSIZE]) {
    irvelog(1, "Created new physical memory instance");
}

pmemory_t::~pmemory_t() {
    delete[] this->m_ram;
}

uint8_t pmemory_t::r(word_t addr) const {
    assert((addr.u < RAMSIZE) && "Invalid memory address");//TODO make this some sort of bus fault exception
    //TODO add MMIO devices that provide data as things progress
    return this->m_ram[addr.u];
}

void pmemory_t::w(word_t addr, uint8_t data) {
    //TODO other MMIO devices
    if (addr == DEBUGADDR) {//Debug output
        //End of line; print the debug string
        if (data == '\n') {
            //NOTE: We print to std::cout instead of using logging because we want to see this even if logging is disabled
            std::cout << "\x1b[1mRISC-V Says: \"" << this->m_debugstr << "\"\x1b[0m" << std::endl;
            this->m_debugstr.clear();
        } else {
            this->m_debugstr.push_back(data);
        }
        return;
    } else {//RAM
        assert((addr.u < RAMSIZE) && "Invalid memory address");//TODO make this some sort of bus fault exception
        this->m_ram[addr.u] = data;
    }
}
