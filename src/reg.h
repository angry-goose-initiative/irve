/* reg.h
 * By Nick Chan
 * 
 * A simple data structure that reduces need for type casting
 * 
*/

#ifndef REG_H
#define REG_H

#include <cstdint>

typedef union {
    uint32_t u;
    int32_t s;
} reg_t;

#endif
