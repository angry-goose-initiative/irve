/* Reg.cpp
 * By Nick Chan
 * 
 * A simple data structure that reduces need for type casting
 * 
*/

#pragma once

#include <cstdint>

class Reg {
public:
    uint32_t u;
    int32_t& s;
    Reg(): u(), s(reinterpret_cast<int32_t&>(u)) {}
};
