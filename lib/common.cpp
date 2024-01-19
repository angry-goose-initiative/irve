/**
 * @file    common.cpp
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
using irve::internal::word_t;//TODO avoid this (only use irve::internal)

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

word_t word_t::bits(uint8_t top_bit, uint8_t bottom_bit) const {
    assert((top_bit >= bottom_bit) && "Bad arguments to bits()");
    assert((top_bit < 32) && "Bad arguments to bits()");
    assert((bottom_bit < 32) && "Bad arguments to bits()");

    //Move the lowest bit desired to the bit 0 position
    word_t intermediate = this->srl(bottom_bit);
    
    //Generate the mask
    uint8_t num_bits = top_bit - bottom_bit + 1;
    word_t mask = (1 << num_bits) - 1;//FIXME on this line this sometimes occurs: "runtime error: signed integer overflow: -2147483648 - 1 cannot be represented in type 'int'"

    //Apply the mask and return
    return intermediate & mask;
}

word_t word_t::sign_extend_from_bit_number(uint8_t bit) const {
    assert((bit < 32) && "Bad argument to sign_extend_from_bit_number()");

    uint8_t shift_amount = 31 - bit;

    //Shift the topmost bit to be extended to the top of the word
    word_t intermediate = *this << shift_amount;

    //ARITHMETIC shift things back down to perform sign extension and return
    return intermediate.sra(shift_amount);
}

word_t word_t::sign_extend_from_size(uint8_t original_size) const {
    assert(original_size && (original_size <= 32) && "Bad argument to sign_extend_from_size()");
    return this->sign_extend_from_bit_number(original_size - 1);
}

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
