/**
 * @brief   Common things for IRVE code
 * 
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  Copyright (C) 2023 Nick Chan\n
 *  See the LICENSE file at the root of the project for licensing info.
*/

#pragma once

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include <cassert>
#include <cstdint>

/* ------------------------------------------------------------------------------------------------
 * Constants/Defines
 * --------------------------------------------------------------------------------------------- */

//Data types (width and signed/unsigned if applicable)
//Based on the funct3 field of load and store instructions
#define DT_BYTE                 ((uint8_t)0b000)
#define DT_HALFWORD             ((uint8_t)0b001)
#define DT_WORD                 ((uint8_t)0b010)
#define DT_SIGNED_BYTE          ((uint8_t)0b000)
#define DT_SIGNED_HALFWORD      ((uint8_t)0b001)
#define DT_UNSIGNED_BYTE        ((uint8_t)0b100)
#define DT_UNSIGNED_HALFWORD    ((uint8_t)0b101)

/* ------------------------------------------------------------------------------------------------
 * Type/Class Declarations
 * --------------------------------------------------------------------------------------------- */

namespace irve::internal {

/**
 * @brief       A RISC-V 32-bit word union, containing abstractions that make operations more explicit and easier
 *
 * Ex. This protects us from accidental arithmetic shifts when we want logical shifts
 *
 * Note this is quite fast because in release builds (using LTO) the compiler will make these 0 cost
 * Plus everything is declared inline so it won't be unbearable in debug builds
*/
struct Word {
    union {
        uint32_t u;
        int32_t s;
    };

    /**
     * @brief       The default Word constructor.
     * @details     Instead of defining the constructor as empty elsewhere, use the default keyword
     *              so that we can put Word in places like in other unions.
    */
    Word() = default;

    // Construct a Word from a uint32_t.
    inline __attribute__((always_inline)) Word(uint32_t data) : u(data) {}

    // Construct a Word from a int32_t.
    inline __attribute__((always_inline)) Word(int32_t data) : s(data) {}
    
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

    /* ---------- Arithmetic ------------------------------------------------------------------- */
    
    // Negate a Word (two's complement).
    inline __attribute__((always_inline)) Word signed_negate() const { return Word(-this->s); }

    // Add two Words.
    inline __attribute__((always_inline)) Word operator+(const Word& other) const { return Word(this->u + other.u); }

    // Add a Word to this one.
    inline __attribute__((always_inline)) Word& operator+=(const Word& other) { return *this = *this + other; }

    // Increment this Word by one.
    inline __attribute__((always_inline)) Word& operator++() { return *this += 1; }

    // Subtract a word from another
    inline __attribute__((always_inline)) Word operator-(const Word& other) const { return Word(this->u - other.u); }

    // Subtract a Word from this one.
    inline __attribute__((always_inline)) Word& operator-=(const Word& other) { return *this = *this - other; }

    // Decrement this Word by one.
    inline __attribute__((always_inline)) Word& operator--() { return *this -= 1; }

    // Multiply two Words together (Lowest 32 bits ONLY).
    inline __attribute__((always_inline)) Word operator*(const Word& other) const { return Word(this->u * other.u); }

    // Multiply a Word by this one (Lowest 32 bits ONLY).
    inline __attribute__((always_inline)) Word& operator*=(const Word& other) { return *this = *this * other; }
    
    /* ---------- Logical ---------------------------------------------------------------------- */

    // Bitwise NOT a Word (one's compliment).
    inline __attribute__((always_inline)) Word operator~() const { return Word(~this->u); }

    // Bitwise OR two Word's together.
    inline __attribute__((always_inline)) Word operator|(const Word& other) const { return Word(this->u | other.u); }

    // Bitwise OR a Word with this one.
    inline __attribute__((always_inline)) Word& operator|=(const Word& other) { return *this = *this | other; }

    // Bitwise AND two Words together.
    inline __attribute__((always_inline)) Word operator&(const Word& other) const { return Word(this->u & other.u); }

    // Bitwise AND a Word with this one.
    inline __attribute__((always_inline)) Word& operator&=(const Word& other) { return *this = *this & other; }

    // Bitwise XOR two Word's together.
    inline __attribute__((always_inline)) Word operator^(const Word& other) const { return Word(this->u ^ other.u); }

    // Bitwise XOR a Word with this one.
    inline __attribute__((always_inline)) Word& operator^=(const Word& other) { return *this = *this ^ other; }

    // Logically shift a Word left by another.
    inline __attribute__((always_inline)) Word operator<<(const Word& other) const {
        assert((other.u < 32) && "Attempt to logically shift left by more than 32 bits!");
        return Word(this->u << other.u);
    }

    // Logically shift this Word left by another.
    inline __attribute__((always_inline)) Word& operator<<=(const Word& other) {
        assert((other.u < 32) && "Attempt to logically shift left by more than 32 bits!");
        return *this = *this << other;
    }

    // Logically shift a Word right by another.
    inline __attribute__((always_inline)) Word srl(const Word& other) const {
        assert((other.u < 32) && "Attempt to logically shift right by more than 32 bits!");
        return Word(this->u >> other.u);
    }

    // Arithmetically shift a Word right by another.
    inline __attribute__((always_inline)) Word sra(const Word& other) const {
        assert((other.u < 32) &&
                "Attempt to arithmetically shift right by more than 32 bits!");
        //TODO ensure this is arithmetic shift
        return Word(this->s >> other.s);
    }
    
    /* ---------- Comparison ------------------------------------------------------------------- */

    // Determine if this Word is nonzero.
    inline __attribute__((always_inline)) bool operator!() const { return *this == 0; }

    // Determine if this Word is equal to another.
    inline __attribute__((always_inline)) bool operator==(const Word& other) const { return this->u == other.u; }

    // Determine if this Word is not equal to another.
    inline __attribute__((always_inline)) bool operator!=(const Word& other) const { return !(*this == other); }
    
    /* ---------- Bonus ------------------------------------------------------------------------ */

    // Extract a single bit from this Word.
    inline __attribute__((always_inline)) Word bit(Word bit) const {
        assert((bit.u < 32) && "Bad argument to bit()");
        return this->srl(bit) & 0b1;
    }

    /**
     * @brief       Extract a range of bits from this Word.
     * @param[in]   top_bit The top bit to extract (inclusive). Must be between bottom_bit and 31
     *              inclusive.
     * @param[in]   bottom_bit The bottom bit to extract (inclusive). Must be between 0 and top_bit
     *              inclusive.
     * @return      A Word containing the bits in the range in the least significant positions.
    */
    Word bits(uint8_t top_bit, uint8_t bottom_bit) const;

    /**
     * @brief       Sign extend this Word from a bit upward to 32 bits (so that all bits above
     *              the bit are the same as the bit).
     * @param[in]   bit The bit to sign extend from. Must be between 0 and 31 inclusive.
     * @return      The resulting Word.
     *
     * Equivalent to sign_extend_from_size(bit + 1)
    */
    Word sign_extend_from_bit_number(uint8_t bit) const;

    /**
     * @brief       Sign extend this Word from a size upward to 32 bits assuming the data within
     *              is original_size bits wide.
     * @param[in]   original_size The size of the original data within. Must be between 1 and 32
     *              inclusive.
     * @return      The resulting Word.
     *
     * Equivalent to sign_extend_from_bit_number(original_size - 1)
    */
    Word sign_extend_from_size(uint8_t original_size) const;
};

static_assert(sizeof(Word) == 4); // A word should be exactly 4 bytes

// A register is just a Word, but seeing it a Reg can be more insightful in other code
using Reg = Word;

/* ------------------------------------------------------------------------------------------------
 * Function Declarations
 * --------------------------------------------------------------------------------------------- */

//TODO make these constexpr too
//TODO should these become part of Word?

/**
 * @brief       Raise an unsigned integer to a power.
 * @param[in]   base The base of the operation.
 * @param[in]   exp The exponent of the operation.
 * @return      base ** exp
*/
uint32_t upow(uint32_t base, uint32_t exp);

/**
 * @brief       Raise a signed integer to a power.
 * @param[in]   base The base of the operation.
 * @param[in]   exp The exponent of the operation.
 * @return      base ** exp
*/
int32_t spow(int32_t base, int32_t exp);

} // namespace irve::internal
