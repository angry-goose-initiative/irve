/* common.h
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Common things for irve code
 *
 * Based on code from rv32esim
*/

#ifndef COMMON_H
#define COMMON_H

/* Includes */

#include <cstdint>

/* Macros */

#define SIGN_EXTEND_TO_32(data, num_bits) (((uint32_t)(((int32_t)((((uint32_t)data) << (32 - num_bits)) & (1ull << 31))) >> (31 - num_bits))) | ((uint32_t)data))

#endif//COMMON_H
