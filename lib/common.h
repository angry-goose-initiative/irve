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

    /**
     * @brief A RISC-V 32-bit word class, containing abstractions that make operations more explicit and easier
    */
    typedef union word_t {
    public:

        /**
         * @brief Default word_t constructor
        */
        word_t() = default;//Instead of defining the constructor as empty elsewhere, use the default keyword so that we can put word_t in places like in other unions

        /**
         * @brief Construct a word_t from a uint32_t
         * @param data The uint32_t to construct the word_t from
        */
        word_t(uint32_t data);

        /**
         * @brief Construct a word_t from a int32_t
         * @param data The int32_t to construct the word_t from
        */
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

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //Arithmetic
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        /**
         * @brief Negate a word_t (two's complement)
         * @return The negated word_t
        */
        word_t signed_negate() const;

        /**
         * @brief Add two word_ts together
         * @param other A reference to the word_t to add to this one
         * @return The sum of the two word_ts
        */
        word_t operator+(const word_t& other) const;

        /**
         * @brief Add a word_t to this one
         * @param other A reference to the word_t to add
         * @return A reference to this word_t
        */
        word_t& operator+=(const word_t& other);

        /**
         * @brief Increment this word_t by one
         * @return A reference to this word_t
        */
        word_t& operator++();

        /**
         * @brief Subtract two word_ts
         * @param other A reference to the word_t to subtract from this one
         * @return The difference of the two word_ts
        */
        word_t operator-(const word_t& other) const;

        /**
         * @brief Subtract a word_t from this one
         * @param other A reference to the word_t to subtract
         * @return A reference to this word_t
        */
        word_t& operator-=(const word_t& other);

        /**
         * @brief Decrement this word_t by one
         * @return A reference to this word_t
        */
        word_t& operator--();

        /**
         * @brief Multiply two word_ts together (Lowest 32 bits ONLY)
         * @param other A reference to the word_t to multiply this one by
         * @return The product of the two word_ts
        */
        word_t operator*(const word_t& other) const;

        /**
         * @brief Multiply a word_t by this one (Lowest 32 bits ONLY)
         * @param other A reference to the word_t to multiply by
         * @return A reference to this word_t
        */
        word_t& operator*=(const word_t& other);
        //TODO different division types
        
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //Logical
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        /**
         * @brief Bitwise NOT a word_t
         * @return The bitwise NOT of this word_t
        */
        word_t operator~() const;

        /**
         * @brief Bitwise OR two word_ts together
         * @param other A reference to the word_t to OR this one with
         * @return The bitwise OR of the two word_ts
        */
        word_t operator|(const word_t& other) const;

        /**
         * @brief Bitwise OR a word_t into this one
         * @param other A reference to the word_t to OR into this one
         * @return A reference to this word_t
        */
        word_t& operator|=(const word_t& other);

        /**
         * @brief Bitwise AND two word_ts together
         * @param other A reference to the word_t to AND this one with
         * @return The bitwise AND of the two word_ts
        */
        word_t operator&(const word_t& other) const;

        /**
         * @brief Bitwise AND a word_t with this one
         * @param other A reference to the word_t to AND with this one
         * @return A reference to this word_t
        */
        word_t& operator&=(const word_t& other);

        /**
         * @brief Bitwise XOR two word_ts together
         * @param other A reference to the word_t to XOR this one with
         * @return The bitwise XOR of the two word_ts
        */
        word_t operator^(const word_t& other) const;

        /**
         * @brief Bitwise XOR a word_t into this one
         * @param other A reference to the word_t to XOR into this one
         * @return A reference to this word_t
        */
        word_t& operator^=(const word_t& other);

        /**
         * @brief Logically shift a word_t left by another
         * @param other A reference to the word_t to shift this one by
         * @return The resulting shifted word_t
        */
        word_t operator<<(const word_t& other) const;

        /**
         * @brief Logically shift this word_t left by another
         * @param other A reference to the word_t to shift this one by
         * @return A reference to this word_t
        */
        word_t& operator<<=(const word_t& other);

        /**
         * @brief Logically shift a word_t right by another
         * @param other A reference to the word_t to shift this one by
         * @return The resulting shifted word_t
        */
        word_t srl(const word_t& other) const;//Have to manually specify

        /**
         * @brief Arithmetically shift a word_t right by another
         * @param other A reference to the word_t to shift this one by
         * @return The resulting shifted word_t
        */
        word_t sra(const word_t& other) const;//Have to manually specify
        
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //Comparison
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        /**
         * @brief Determine if this word_t is nonzero
         * @return True if this word_t is nonzero, false otherwise
        */
        bool operator!() const;

        /**
         * @brief Determine if this word_t is equal to another
         * @param other A reference to the word_t to compare this one with
         * @return True if this word_t is equal to the other, false otherwise
        */
        bool operator==(const word_t& other) const;

        /**
         * @brief Determine if this word_t is not equal to another
         * @param other A reference to the word_t to compare this one with
         * @return True if this word_t is not equal to the other, false otherwise
        */
        bool operator!=(const word_t& other) const;
        //TODO others
        
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //Bonus!
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        /**
         * @brief Raise this word_t to the power of another
         * @param other A reference to the word_t to raise this one by (exponent)
         * @return The resulting word_t
        */
        word_t pow(const word_t& other) const;//Integer power

        /**
         * @brief Extract a single bit from this word_t
         * @param bit The bit to extract. Must be between 0 and 31 inclusive.
         * @return A word_t containing the bit in the least significant position
        */
        word_t bit(uint8_t bit) const;//Get a single bit//TODO should we overload operator()

        /**
         * @brief Extract a range of bits from this word_t
         * @param top_bit The top bit to extract (inclusive). Must be between bottom_bit and 31 inclusive.
         * @param bottom_bit The bottom bit to extract (inclusive). Must be between 0 and top_bit inclusive.
         * @return A word_t containing the bits in the range in the least significant positions
        */
        word_t bits(uint8_t top_bit, uint8_t bottom_bit) const;//Get a range of bits (inclusive)//TODO should we overload operator()

        /**
         * @brief Sign extend this word_t from a bit upward to 32 bits (so that all bits above the bit are the same as the bit)
         * @param bit The bit to sign extend from. Must be between 0 and 31 inclusive
         * @return The resulting word_t
         *
         * Equivalent to sign_extend_from_size(bit + 1)
        */
        word_t sign_extend_from_bit_number(uint8_t bit) const;//Sign extend from a bit upward to 32 bits

        /**
         * @brief Sign extend this word_t from a size upward to 32 bits assuming the data within is original_size bits wide
         * @param original_size The size of the original data within. Must be between 1 and 32 inclusive
         * @return The resulting word_t
         *
         * Equivalent to sign_extend_from_bit_number(original_size - 1)
        */
        word_t sign_extend_from_size(uint8_t original_size) const;//Sign extend from a size to 32 bits

        ///Access the contents of this word_t as an unsigned integer
        uint32_t u;

        ///Access the contents of this word_t as a signed integer
        int32_t s;
    } word_t;

    typedef word_t reg_t;

    /* Function/Class Declarations */

    //TODO make these constexpr too
    //TODO should these become part of word_t?
    
    /**
     * @brief Raise an unsigned integer to a power
     * @param base The base of the operation
     * @param exp The exponent of the operation
     * @return base ** exp
    */
    uint32_t upow(uint32_t base, uint32_t exp);//Unsigned integer power

    /**
     * @brief Raise a signed integer to a power
     * @param base The base of the operation
     * @param exp The exponent of the operation
     * @return base ** exp
    */
    int32_t spow(int32_t base, int32_t exp);//Signed integer power

}//namespace irve::internal

#endif//COMMON_H
