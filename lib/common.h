/**
 * @file    common.h
 * @brief   Common things for irve code
 * 
 * @copyright Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 * 
 * TODO longer description
 *
 * Based on code from rv32esim
 *
*/

#ifndef COMMON_H
#define COMMON_H

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include <cassert>
#include <cstdint>

/* ------------------------------------------------------------------------------------------------
 * Constants/Defines
 * --------------------------------------------------------------------------------------------- */

// DT prefix stands for 'data type'
#define DT_BYTE                0b000
#define DT_HALFWORD            0b001
#define DT_WORD                0b010
#define DT_SIGNED_BYTE         0b000
#define DT_SIGNED_HALFWORD     0b001
#define DT_UNSIGNED_BYTE       0b100
#define DT_UNSIGNED_HALFWORD   0b101

/* ------------------------------------------------------------------------------------------------
 * Type/Class Declarations
 * --------------------------------------------------------------------------------------------- */

namespace irve::internal {//Exception: Not irve::internal::common since it is, after all, "common"

    /**
     * @brief A RISC-V 32-bit word union, containing abstractions that make operations more explicit and easier
     *
     * Ex. This protects us from accidental arithmetic shifts when we want logical shifts
     *
     * Note this is quite fast because in release builds (using LTO) the compiler will make these 0 cost
     * Plus everything is declared inline so it won't be unbearable in debug builds
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
        inline word_t(uint32_t data) : u(data) {}

        /**
         * @brief Construct a word_t from a int32_t
         * @param data The int32_t to construct the word_t from
        */
        inline word_t(int32_t data) : s(data) {}
        
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
        inline word_t signed_negate() const {
            return word_t(-this->s);
        }

        /**
         * @brief Add two word_ts together
         * @param other A reference to the word_t to add to this one
         * @return The sum of the two word_ts
        */
        inline word_t operator+(const word_t& other) const {
            return word_t(this->u + other.u);
        }

        /**
         * @brief Add a word_t to this one
         * @param other A reference to the word_t to add
         * @return A reference to this word_t
        */
        inline word_t& operator+=(const word_t& other) {
            return *this = *this + other;
        }

        /**
         * @brief Increment this word_t by one
         * @return A reference to this word_t
        */
        inline word_t& operator++() {
            return *this += 1;
        }

        /**
         * @brief Subtract two word_ts
         * @param other A reference to the word_t to subtract from this one
         * @return The difference of the two word_ts
        */
        inline word_t operator-(const word_t& other) const {
            //TODO ensure this is the same for signed
            return word_t(this->u - other.u);
        }

        /**
         * @brief Subtract a word_t from this one
         * @param other A reference to the word_t to subtract
         * @return A reference to this word_t
        */
        inline word_t& operator-=(const word_t& other) {
            return *this = *this - other;
        }

        /**
         * @brief Decrement this word_t by one
         * @return A reference to this word_t
        */
        inline word_t& operator--() {
            return *this -= 1;
        }

        /**
         * @brief Multiply two word_ts together (Lowest 32 bits ONLY)
         * @param other A reference to the word_t to multiply this one by
         * @return The product of the two word_ts
        */
        inline word_t operator*(const word_t& other) const {
            return word_t(this->u * other.u);
        }

        /**
         * @brief Multiply a word_t by this one (Lowest 32 bits ONLY)
         * @param other A reference to the word_t to multiply by
         * @return A reference to this word_t
        */
        inline word_t& operator*=(const word_t& other) {
            return *this = *this * other;
        }
        
        //TODO different division types
        
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //Logical
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        /**
         * @brief Bitwise NOT a word_t
         * @return The bitwise NOT of this word_t
        */
        inline word_t operator~() const {
            return word_t(~this->u);
        }

        /**
         * @brief Bitwise OR two word_ts together
         * @param other A reference to the word_t to OR this one with
         * @return The bitwise OR of the two word_ts
        */
        inline word_t operator|(const word_t& other) const {
            return word_t(this->u | other.u);
        }

        /**
         * @brief Bitwise OR a word_t into this one
         * @param other A reference to the word_t to OR into this one
         * @return A reference to this word_t
        */
        inline word_t& operator|=(const word_t& other) {
            return *this = *this | other;
        }

        /**
         * @brief Bitwise AND two word_ts together
         * @param other A reference to the word_t to AND this one with
         * @return The bitwise AND of the two word_ts
        */
        inline word_t operator&(const word_t& other) const {
            return word_t(this->u & other.u);
        }

        /**
         * @brief Bitwise AND a word_t with this one
         * @param other A reference to the word_t to AND with this one
         * @return A reference to this word_t
        */
        inline word_t& operator&=(const word_t& other) {
            return *this = *this & other;
        }

        /**
         * @brief Bitwise XOR two word_ts together
         * @param other A reference to the word_t to XOR this one with
         * @return The bitwise XOR of the two word_ts
        */
        inline word_t operator^(const word_t& other) const {
            return word_t(this->u ^ other.u);
        }

        /**
         * @brief Bitwise XOR a word_t into this one
         * @param other A reference to the word_t to XOR into this one
         * @return A reference to this word_t
        */
        inline word_t& operator^=(const word_t& other) {
            return *this = *this ^ other;
        }

        /**
         * @brief Logically shift a word_t left by another
         * @param other A reference to the word_t to shift this one by
         * @return The resulting shifted word_t
        */
        inline word_t operator<<(const word_t& other) const {
            assert((other.u < 32) && "Attempt to logically shift left by more than 32 bits!");
            return word_t(this->u << other.u);
        }

        /**
         * @brief Logically shift this word_t left by another
         * @param other A reference to the word_t to shift this one by
         * @return A reference to this word_t
        */
        inline word_t& operator<<=(const word_t& other) {
            assert((other.u < 32) && "Attempt to logically shift left by more than 32 bits!");
            return *this = *this << other;
        }

        /**
         * @brief Logically shift a word_t right by another
         * @param other A reference to the word_t to shift this one by
         * @return The resulting shifted word_t
        */
        inline word_t srl(const word_t& other) const {
            assert((other.u < 32) && "Attempt to logically shift right by more than 32 bits!");
            return word_t(this->u >> other.u);
        }

        /**
         * @brief Arithmetically shift a word_t right by another
         * @param other A reference to the word_t to shift this one by
         * @return The resulting shifted word_t
        */
        inline word_t sra(const word_t& other) const {
            assert((other.u < 32) && "Attempt to arithmetically shift right by more than 32 bits!");
            //TODO ensure this is arithmetic shift
            return word_t(this->s >> other.s);
        }
        
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //Comparison
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        /**
         * @brief Determine if this word_t is nonzero
         * @return True if this word_t is nonzero, false otherwise
        */
        inline bool operator!() const {
            return *this == 0;
        }

        /**
         * @brief Determine if this word_t is equal to another
         * @param other A reference to the word_t to compare this one with
         * @return True if this word_t is equal to the other, false otherwise
        */
        inline bool operator==(const word_t& other) const {
            return this->u == other.u;
        }

        /**
         * @brief Determine if this word_t is not equal to another
         * @param other A reference to the word_t to compare this one with
         * @return True if this word_t is not equal to the other, false otherwise
        */
        inline bool operator!=(const word_t& other) const {
            return !(*this == other);
        }

        //TODO others
        
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //Bonus!
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        /**
         * @brief Extract a single bit from this word_t
         * @param bit The bit to extract. Must be between 0 and 31 inclusive.
         * @return A word_t containing the bit in the least significant position
        */
        inline word_t bit(word_t bit) const {//TODO should we overload operator()
            assert((bit.u < 32) && "Bad argument to bit()");
            return this->srl(bit) & 0b1;
        }

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

    ///A register is just a word_t, but seeing it a reg_t can be more insightful in other code
    typedef word_t reg_t;

/* ------------------------------------------------------------------------------------------------
 * Function Declarations
 * --------------------------------------------------------------------------------------------- */

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
