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

/* Types */

namespace irve::internal {//Exception: Not irve::internal::common since it is, after all, "common"

typedef union word_t {
public:
    word_t();
    word_t(uint32_t data);
    word_t(int32_t data);
    //NOTE: NOT providing these transparent type conversion functions to make things more explicit
    /*
    operator uint32_t() const;
    operator int32_t() const;
    operator bool() const;//This is fine though
    */

    //Operator overloads for where the operation is the same for both signed and unsigned
    //Unary - is NOT okay since it is signed only
    //NOTE: * is okay since the bottom 32 bits are the same for both signed and unsigned and we only return the bottom 32 bits
    //Right shift is NOT okay since we don't know if it is arithmetic or logical
    //Division is NOT okay since we don't know if it is signed or unsigned
    //For comparison, really only == and != are safe
    
    //TODO make all of these constexpr and inline

    //Arithmetic
    word_t signed_negate() const;
    word_t operator+(const word_t& other) const;
    word_t& operator+=(const word_t& other);
    word_t& operator++();
    word_t operator-(const word_t& other) const;
    word_t& operator-=(const word_t& other);
    word_t& operator--();
    word_t operator*(const word_t& other) const;
    word_t& operator*=(const word_t& other);
    //TODO different division types
    
    //Logical
    word_t operator~() const;
    word_t operator|(const word_t& other) const;
    word_t& operator|=(const word_t& other);
    word_t operator&(const word_t& other) const;
    word_t& operator&=(const word_t& other);
    word_t operator^(const word_t& other) const;
    word_t& operator^=(const word_t& other);
    word_t operator<<(const word_t& other) const;
    word_t& operator<<=(const word_t& other);
    word_t srl(const word_t& other) const;//Have to manually specify
    word_t sra(const word_t& other) const;//Have to manually specify
    
    //Comparison
    bool operator!() const;
    bool operator==(const word_t& other) const;
    bool operator!=(const word_t& other) const;
    //TODO others
    
    //Bonus!
    word_t pow(const word_t& other) const;//Integer power
    word_t bit(uint8_t bit) const;//Get a single bit//TODO should we overload operator()
    word_t bits(uint8_t top_bit, uint8_t bottom_bit) const;//Get a range of bits (inclusive)//TODO should we overload operator()
    word_t sign_extend_upward_from_bit(uint8_t bit) const;//Sign extend from a bit upward

    uint32_t u;
    int32_t s;
} word_t;

typedef word_t reg_t;

/* Function/Class Declarations */

//TODO make these constexpr too
uint32_t upow(uint32_t base, uint32_t exp);//Unsigned integer power
int32_t spow(int32_t base, int32_t exp);//Signed integer power

}//namespace irve::internal

#endif//COMMON_H
