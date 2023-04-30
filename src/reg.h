/* reg.h
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
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
