/* memory.cpp
 * By Nick Chan
 * 
 * TODO description
 * 
*/

#include "memory.h"

#include <cassert>
#include <cmath>

#define INST_COUNT 0//We only log at init
#include "logging.h"

// All of memory is initialized to 0
memory_t::memory_t(): m_mem() {
    irvelog(1, "Created new Memory instance");
}

// Read from memory
int32_t memory_t::r(const uint32_t addr, const int8_t func3) const {

    assert((func3 >= 0b000) && (func3 <= 0b111) && "Invalid func3");
    assert((addr < MEMSIZE) && "Invalid memory address");  // TODO throw exceptions to be caught?
    assert(((addr + pow(2, func3%4) - 1) < MEMSIZE) && "Invalid memory address");

    // MSB of func3 determines signed/unsigned
    const int isUnsigned{func3 >> 2};
    // 2^(func3[1:0]) is the number of bytes
    int byte{static_cast<int>(pow(2, func3%4)) - 1};

    int32_t data{};
    data = static_cast<int32_t>(m_mem[(addr + byte)]) << (8*byte);

    // The casting above sign extends so if the number is unsigned then we need
    // to remove the sign extention
    if(isUnsigned) data &= (0xFF << (8*byte));
    --byte;
    
    for(; byte > -1; --byte) {
        data |= (static_cast<int32_t>(m_mem[addr + byte]) & 0xFF) << (8*byte);
    }

    return data;
}

// Write to memory
void memory_t::w(const uint32_t addr, const int8_t func3, const int32_t data) {

    assert((func3 >= 0b000) && (func3 <= 0b010) && "Invalid func3");
    assert((addr < MEMSIZE) && "Invalid memory address"); // TODO throw exceptions to be caught?
    assert(((addr + pow(2, func3%4) - 1) < MEMSIZE) && "Invalid memory address");

    // 2^(func3[1:0]) is the number of bytes
    int byte{static_cast<int>(pow(2, func3%4))};

    for(int i{}; i<byte; ++i) {
        m_mem[addr + i] = static_cast<int8_t>(data >> 8*i);
    }
}

// TODO integrate with logging or delete
// Prints the 8 bytes at and following the specified address in hex
void memory_t::p(const uint32_t addr) const {

    for(int byte{}; byte<8; ++byte) {
        // top 4 bits
        std::cout << std::hex << ((m_mem[addr + byte] >> 4) & 0xF);
        // bottom 4 bits
        std::cout << std::hex << (m_mem[addr + byte] & 0xF);
        std::cout << " ";
    }
    std::cout << std::endl;
}
