/* Memory.cpp
 * 
*/

#include "Memory.hpp"

// All of memory is initialized to 0
Memory::Memory(): mem() {}

// Read from memory
int32_t Memory::r(const uint32_t addr, const int8_t func3) const {

    // TODO: check that address is valid

    // MSB of func3 determines signed/unsigned
    const int isUnsigned{func3 >> 2};
    // 2^(func3[1:0]) is the number of bytes
    int byte{static_cast<int>(pow(2, func3%4)) - 1};

    int32_t data{};
    data = static_cast<int32_t>(mem[(addr + byte)%MEMSIZE]) << (8*byte);

    // The casting above sign extends so if the number is unsigned then we need
    // to remove the sign extention
    if(isUnsigned) data &= (0xFF << (8*byte));
    --byte;
    
    for(; byte > -1; --byte) {
        data |= (static_cast<int32_t>(mem[addr + byte]%MEMSIZE) & 0xFF) << (8*byte);
    }

    return data;
}

// Write to memory
void Memory::w(const uint32_t addr, const int8_t func3, const int32_t data) {

    // TODO: check that address is valid

    // 2^(func3[1:0]) is the number of bytes
    int byte{static_cast<int>(pow(2, func3%4))};

    for(int i{}; i<byte; ++i) {
        mem[addr + i] = static_cast<int8_t>(data >> 8*i);
    }
}

// Prints the 8 bytes at and following the specified address in hex
void Memory::p(const uint32_t addr) const {

    for(int byte{}; byte<8; ++byte) {
        // top 4 bits
        std::cout << std::hex << ((mem[addr + byte] >> 4) & 0xF);
        // bottom 4 bits
        std::cout << std::hex << (mem[addr + byte] & 0xF);
        std::cout << " ";
    }
    std::cout << std::endl;
}
