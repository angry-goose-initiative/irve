/**
 * @brief   Random emulator initialization
 * 
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  See the LICENSE file at the root of the project for licensing info.
 * 
 * Facilities for random initialization of certain parts of the emulator
 * (depending on configuration options) in order to help catch bugs
 *
*/

#pragma once

#include "config.h"

#if IRVE_INTERNAL_CONFIG_FUZZISH
#   include <cstddef>
#   include <cstdint>
#   include <cstdlib>
#else
#   include <cstring>
#endif

namespace irve::internal::fuzzish {

#if IRVE_INTERNAL_CONFIG_FUZZISH

inline void meminit(void *ptr, std::size_t size) {
    uint8_t* byte_ptr{static_cast<uint8_t*>(ptr)};
    for (std::size_t i = 0; i < size; ++i) {
        byte_ptr[i] = std::rand();
    }
}

inline int rand() { return std::rand(); }

#else

inline void meminit(void *ptr, std::size_t size) { std::memset(ptr, 0, size); }
inline int rand() { return 0; }

#endif

} // namespace irve::internal::fuzzish
