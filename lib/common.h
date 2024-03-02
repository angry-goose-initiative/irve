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
 * A RISC-V 32-bit word union, containing abstractions that make operations more explicit and easier
 *
 * Ex. This protects us from accidental arithmetic shifts when we want logical shifts
 *
 * Note this is quite fast because in release builds (using LTO) the compiler will make these 0 cost
 * Plus everything is inline (implicitly, see https://en.cppreference.com/w/cpp/language/inline) so it
 * won't be unbearable in debug builds
 *
 * Word MUST ONLY CONTAIN A UNION OF A SIGNED AND UNSIGNED 32-BIT INTEGER SO THAT IT ENDS UP COMPILING
 * DOWN TO REGULAR INTEGER OPERATIONS. We do this so we can prevent accidental casts and operations where
 * it is unclear if they are the signed or unsigned variant. We also assert later on that the sizeof(Word)
 * is 4 bytes as a sanity check that this is the case.
 *
 * This is important since when implementing RISC-V instructions we want to be very careful of the
 * difference between arithmetic and logical shifts for example.
 *
 * We would just add certain functions to the standard types but this C++ is not Rust sadly
*/
struct Word {
    union {
        uint32_t u;
        int32_t s;
    };

    Word() = default;
    Word(uint32_t data) : u(data) {}
    Word(int32_t data) : s(data) {}
    
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

    /* ---------- Arithmetic ------------------------------------------------------------------- */
    
    // Negate a Word (two's complement).
    Word signed_negate() const { return Word(-this->s); }

    // Add two Words.
    Word operator+(const Word& other) const { return Word(this->u + other.u); }

    // Add a Word to this one.
    Word& operator+=(const Word& other) { return *this = *this + other; }

    // Increment this Word by one.
    Word& operator++() { return *this += 1; }

    // Subtract a word from another
    Word operator-(const Word& other) const { return Word(this->u - other.u); }

    // Subtract a Word from this one.
    Word& operator-=(const Word& other) { return *this = *this - other; }

    // Decrement this Word by one.
    Word& operator--() { return *this -= 1; }

    // Multiply two Words together (Lowest 32 bits ONLY).
    Word operator*(const Word& other) const { return Word(this->u * other.u); }

    // Multiply a Word by this one (Lowest 32 bits ONLY).
    Word& operator*=(const Word& other) { return *this = *this * other; }
    
    /* ---------- Logical ---------------------------------------------------------------------- */

    // Bitwise NOT a Word (one's compliment).
    Word operator~() const { return Word(~this->u); }

    // Bitwise OR two Word's together.
    Word operator|(const Word& other) const { return Word(this->u | other.u); }

    // Bitwise OR a Word with this one.
    Word& operator|=(const Word& other) { return *this = *this | other; }

    // Bitwise AND two Words together.
    Word operator&(const Word& other) const { return Word(this->u & other.u); }

    // Bitwise AND a Word with this one.
    Word& operator&=(const Word& other) { return *this = *this & other; }

    // Bitwise XOR two Word's together.
    Word operator^(const Word& other) const { return Word(this->u ^ other.u); }

    // Bitwise XOR a Word with this one.
    Word& operator^=(const Word& other) { return *this = *this ^ other; }

    // Logically shift a Word left by another.
    Word operator<<(const Word& other) const {
        assert((other.u < 32) && "Attempt to logically shift left by more than 32 bits!");
        return Word(this->u << other.u);
    }

    // Logically shift this Word left by another.
    Word& operator<<=(const Word& other) {
        assert((other.u < 32) && "Attempt to logically shift left by more than 32 bits!");
        return *this = *this << other;
    }

    // Logically shift a Word right by another.
    Word srl(const Word& other) const {
        assert((other.u < 32) && "Attempt to logically shift right by more than 32 bits!");
        return Word(this->u >> other.u);
    }

    // Arithmetically shift a Word right by another.
    Word sra(const Word& other) const {
        assert((other.u < 32) &&
                "Attempt to arithmetically shift right by more than 32 bits!");
        //TODO ensure this is arithmetic shift
        return Word(this->s >> other.s);
    }
    
    /* ---------- Comparison ------------------------------------------------------------------- */

    // Determine if this Word is nonzero.
    bool operator!() const { return *this == 0; }

    // Determine if this Word is equal to another.
    bool operator==(const Word& other) const { return this->u == other.u; }

    // Determine if this Word is not equal to another.
    bool operator!=(const Word& other) const { return !(*this == other); }
    
    /* ---------- Bonus ------------------------------------------------------------------------ */

    // Extract a single bit from this Word.
    Word bit(const Word bit) const {
        assert((bit.u < 32U) && "Bad argument to bit()");
        return this->srl(bit) & 0b1;
    }

    // Extract a range of bits from this Word.
    Word bits(const uint8_t top_bit, const uint8_t bottom_bit) const {
        assert((bottom_bit <= top_bit && top_bit < 32U) && "Bad arguments to bits()");

        // Move the lowest bit desired to the bit 0 position
        const Word intermediate = this->srl(bottom_bit);
    
        // Generate the mask
        const uint8_t num_bits = top_bit - bottom_bit + 1U;
        const Word mask = (1U << num_bits) - 1U;

        // Apply the mask and return
        return intermediate & mask;
    }

    // Sign extend this Word from a bit upward to 32 bits (so that all bits above the bit are the
    // same as the bit).
    Word sign_extend_from_bit_number(const uint8_t bit) const {
        assert((bit < 32U) && "Bad argument to sign_extend_from_bit_number()");

        const uint8_t shift_amount = 31U - bit;

        //Shift the topmost bit to be extended to the top of the word
        const Word intermediate = *this << shift_amount;

        //ARITHMETIC shift things back down to perform sign extension and return
        return intermediate.sra(shift_amount);
    }
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
