/**
 * @brief   The memory map for IRVE
 * 
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  Copyright (C) 2023 Nick Chan\n
 *  See the LICENSE file at the root of the project for licensing info.
*/

#pragma once

#include <cstdint>

namespace irve::internal::mmap {

using Addr34 = uint64_t; // TODO(Nick) Move somewhere else

// Structure to hold addresses of memory mapped regions
template<Addr34 start, Addr34 end>
struct MRegion {
    static_assert(start <= end);
    static_assert((start & 0xFFFFFFFC00000000) == 0); // Ensure address is only 34 bits
    static_assert((end & 0xFFFFFFFC00000000) == 0); // Ensure address is only 34 bits
    inline bool in_region(const Addr34 addr) const {
        return start <= addr && addr <= end;
    }
    const Addr34 START{start};
    const Addr34 END{end};
    const uint64_t SIZE{end - start + 1};
};

constexpr MRegion<0x00000000U, 0x03FFFFFFU> USER_RAM;
constexpr MRegion<0xC0000000U, 0xC3FFFFFFU> KERNEL_RAM;
constexpr MRegion<0xF0000000U, 0xF000BFFFU> ACLINT;
constexpr MRegion<0xF1000000U, 0xF1000007U> UART;
constexpr MRegion<0xFFFFFFFFU, 0xFFFFFFFFU> DEBUG;

} // irve::internal::mmap
