/* reg.h
 * By Nick Chan
 * 
 * A simple data structure that reduces need for type casting
 * 
*/

#ifndef REG_H
#define REG_H

#include <cstdint>

class reg_t {
public:
    uint32_t u;
    int32_t& s;
    reg_t(): u(), s(reinterpret_cast<int32_t&>(u)) {}
};

#endif
