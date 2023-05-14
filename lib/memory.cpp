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
#include <iostream>

#include "CSR.h"

#include "common.h"
#include "rvexception.h"

#define INST_COUNT 0//We only log at init
#include "logging.h"

using namespace irve::internal;

/* Function Implementations */

//Virtual memory

// All of memory is initialized to 0
memory::memory_t::memory_t(CSR::CSR_t& CSR_ref): m_mem(), m_CSR_ref(CSR_ref) {
    irvelog(1, "Created new Memory instance");
}

// Read from memory
word_t memory::memory_t::r(word_t addr, int8_t func3) const {


    // inaccessable address exceptions:
    //  - vacant addresses are never accessible

    // should memory accesses be word-aligned? (if misaligned is allowed caching will be harder)

    // refuse to run RVTSO binaries?

    assert((func3 >= 0b000) && (func3 <= 0b111) && "Invalid func3");
    //TODO from now on exceptions for invalid physical memory accesses will be thrown by pmemory_t instead
    //TODO we will have to handle page faults here though
    //assert((addr < MEMSIZE) && "Invalid memory address");  // TODO throw exceptions to be caught?
    //assert(((addr + pow(2, func3%4) - 1) < MEMSIZE) && "Invalid memory address");

    //FIXME to pass the unit test we need to ask physical memory if it is valid to read a particular size from an address

    if (((func3 & 0b11) == 0b001) && ((addr.u % 2) != 0)) {
        invoke_rv_exception_with_cause(LOAD_ADDRESS_MISALIGNED_EXCEPTION);
    } else if ((func3 == 0b010) && ((addr.u % 4) != 0)) {
        invoke_rv_exception_with_cause(LOAD_ADDRESS_MISALIGNED_EXCEPTION);
    }

    // MSB of func3 determines signed/unsigned
    bool isUnsigned = func3 >> 2;
    // 2^(func3[1:0]) is the number of bytes
    int8_t byte = (int8_t)(spow(2, func3 % 4) - 1);

    word_t data = m_mem.r(addr + byte) << (byte * 8);

    // The casting above sign extends so if the number is unsigned then we need
    // to remove the sign extention
    if (isUnsigned) data &= (0xFF << (byte * 8));
    --byte;
    
    for(; byte > -1; --byte) {
        data |= (m_mem.r(addr + byte) & 0xFF) << (byte * 8);
    }

    return data;
}

// Write to memory
void memory::memory_t::w(word_t addr, int8_t func3, word_t data) {
    if (((func3 & 0b11) == 0b001) && ((addr.u % 2) != 0)) {
        invoke_rv_exception_with_cause(STORE_OR_AMO_ADDRESS_MISALIGNED_EXCEPTION);
    } else if ((func3 == 0b010) && ((addr.u % 4) != 0)) {
        invoke_rv_exception_with_cause(STORE_OR_AMO_ADDRESS_MISALIGNED_EXCEPTION);
    }

    //FIXME to pass the unit test we need to ask physical memory if it is valid to read a particular size from an address

    assert((func3 >= 0b000) && (func3 <= 0b010) && "Invalid func3");
    //TODO from now on exceptions for invalid physical memory accesses will be thrown by pmemory_t instead
    //TODO we will have to handle page faults here though
    //assert((addr < MEMSIZE) && "Invalid memory address"); // TODO throw exceptions to be caught?
    //assert(((addr + pow(2, func3%4) - 1) < MEMSIZE) && "Invalid memory address");

    // 2^(func3[1:0]) is the number of bytes
    int8_t byte = (int8_t)spow(2, func3 % 4);

    for(int i{}; i<byte; ++i) {
        m_mem.w(addr + i, (data.srl(8 * i) & 0xFF).u);
    }
}

// TODO integrate with logging or delete
// Prints the 8 bytes at and following the specified address in hex
void memory::memory_t::p(word_t addr) const {

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

memory::pmemory_t::pmemory_t(): m_ram(new uint8_t[RAMSIZE]) {
    irvelog(1, "Created new physical memory instance");
}

memory::pmemory_t::~pmemory_t() {
    delete[] this->m_ram;
}

uint8_t memory::pmemory_t::r(word_t addr) const {
    if (addr.u >= RAMSIZE) {
        invoke_rv_exception_with_cause(LOAD_ACCESS_FAULT_EXCEPTION);
    }

    //TODO add MMIO devices that provide data as things progress
    
    return this->m_ram[addr.u];
}

void memory::pmemory_t::w(word_t addr, uint8_t data) {
    //TODO other MMIO devices
    
    if (addr == DEBUGADDR) {//Debug output
        //End of line; print the debug string
        if (data == '\n') {
            //NOTE: We print to std::cout instead of using logging because we want to see this even if logging is disabled
            irvelog_always_stdout(0, "\x1b[92mRISC-V Says\x1b[0m: \"\x1b[1m%s\x1b[0m\"", this->m_debugstr.c_str());
            this->m_debugstr.clear();
        } else {
            this->m_debugstr.push_back(data);
        }

        return;
    } else {//RAM
        if (addr.u >= RAMSIZE) {
            invoke_rv_exception_with_cause(STORE_OR_AMO_ACCESS_FAULT_EXCEPTION);
        }

        this->m_ram[addr.u] = data;
    }
}
