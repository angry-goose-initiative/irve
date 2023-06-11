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

/* Constants and Defines */

// DT prefix stands for 'data type'
#define DT_BYTE                0b000
#define DT_HALFWORD            0b001
#define DT_WORD                0b010
#define DT_SIGNED_BYTE         0b000
#define DT_SIGNED_HALFWORD     0b001
#define DT_UNSIGNED_BYTE       0b100
#define DT_UNSIGNED_HALFWORD   0b101

/* Types */

namespace irve::internal {//Exception: Not irve::internal::common since it is, after all, "common"

    typedef union word_t {
    public:
        word_t() = default;//Instead of defining the constructor as empty elsewhere, use the default keyword so that we can put word_t in places like in other unions
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
        word_t sign_extend_from_bit_number(uint8_t bit) const;//Sign extend from a bit upward to 32 bits
        word_t sign_extend_from_size(uint8_t original_size) const;//Sign extend from a size to 32 bits

        // Interpret word as unsigned integer
        uint32_t u;

        // Interpret word as signed integer
        int32_t s;
    } word_t;

    typedef word_t reg_t;

    /* Function/Class Declarations */

    //TODO make these constexpr too
    //TODO should these become part of word_t?
    uint32_t upow(uint32_t base, uint32_t exp);//Unsigned integer power
    int32_t spow(int32_t base, int32_t exp);//Signed integer power

}//namespace irve::internal

#endif//COMMON_H
