/* Memory.hpp
 * 
*/

#pragma once

#include <iostream>

#define MEMSIZE 1024

class Memory {
private:
    // The "physical" memory
    int8_t mem[MEMSIZE];
public:
    Memory();
    int32_t r(const uint32_t addr, const int8_t func3) const;
    void w(const uint32_t addr, const int8_t func3, const int32_t data);
    void p(const uint32_t addr) const;
};
