/**
 * @brief   Common things for IRVE code
 * 
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  Copyright (C) 2023 Nick Chan\n
 *  See the LICENSE file at the root of the project for licensing info.
*/

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include "common.h"

#include <cstdint>
#include <cassert>
#include <stdexcept>

using namespace irve::internal;

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

uint32_t irve::internal::upow(uint32_t base, uint32_t exponent) {
    uint32_t result = 1;
    while (exponent) {
        if (exponent & 0b1) {
            result *= base;
        }
        base *= base;
        exponent >>= 1;//IMPORTANT: This is a LOGICAL shift
    }
    return result;
}

int32_t irve::internal::spow(int32_t base, int32_t exponent) {
    if (exponent < 0) {
        int32_t denominator = spow(base, -exponent);
        if (!denominator) {
            throw std::runtime_error("Exponentiation by negative number resulted in division by zero");
        } else {
            return 1 / denominator;
        }
    } else if (base < 0) {
        int32_t abs_result = upow(-base, exponent);
        if (exponent & 0b1) {//If exponent is odd
            return -abs_result;
        } else {
            return abs_result;
        }
    } else {//Both positive
        return (int32_t)upow(base, exponent);
    }
}
