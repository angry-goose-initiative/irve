/* fuzzish.h
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Facilities for random initialization of certain parts of the emulator
 * (depending on configuration options)
 *
*/

#ifndef FUZZISH_H
#define FUZZISH_H

/* Includes */

#include "config.h"

#include <cstdint>

#if IRVE_FUZZISH
#include <cstdlib>
#else
#include <cstring>
#endif

/* Constants And Defines */

#if IRVE_FUZZISH
//TODO be more efficient (use all 32 bits)
#define irve_fuzzish_meminit(ptr, size_bytes) do { \
    uint8_t* byte_ptr = (uint8_t*)ptr; \
    for (size_t i = 0; i < size_bytes; i++) { \
        byte_ptr[i] = std::rand(); \
    } \
} while (0)
#define irve_fuzzish_rand(__VA_ARGS__) std::rand(__VA_ARGS__)
#else
#define irve_fuzzish_meminit(ptr, size_bytes) do { std::memset(ptr, 0, size_bytes); } while (0)
#define irve_fuzzish_rand() 0
#endif

#endif//FUZZISH_H
