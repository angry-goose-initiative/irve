/* common.cpp
 * Copyright (C) TODO John Jekel
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Function implementations for common.h
 *
*/

/* Includes */

#include "common.h"

#include <cstdint>
#include <cassert>

/* Function Implementations */

word_t::word_t() {}

word_t::word_t(uint32_t u) : u(u) {}

word_t::word_t(int32_t s) : s(s) {}

word_t word_t::signed_negate() const {
    return word_t(-this->s);
}

word_t word_t::operator+(const word_t& other) const {
    return word_t(this->u + other.u);
}

word_t& word_t::operator+=(const word_t& other) {
    return *this = *this + other;
}

word_t& word_t::operator++() {
    return *this += 1;
}

word_t word_t::operator-(const word_t& other) const {
    //TODO ensure this is the same for signed
    return word_t(this->u - other.u);
}

word_t& word_t::operator-=(const word_t& other) {
    return *this = *this - other;
}

word_t& word_t::operator--() {
    return *this -= 1;
}

word_t word_t::operator*(const word_t& other) const {
    return word_t(this->u * other.u);
}

word_t& word_t::operator*=(const word_t& other) {
    return *this = *this * other;
}

word_t word_t::operator~() const {
    return word_t(~this->u);
}

word_t word_t::operator|(const word_t& other) const {
    return word_t(this->u | other.u);
}

word_t& word_t::operator|=(const word_t& other) {
    return *this = *this | other;
}

word_t word_t::operator&(const word_t& other) const {
    return word_t(this->u & other.u);
}

word_t& word_t::operator&=(const word_t& other) {
    return *this = *this & other;
}

word_t word_t::operator^(const word_t& other) const {
    return word_t(this->u ^ other.u);
}

word_t& word_t::operator^=(const word_t& other) {
    return *this = *this ^ other;
}

word_t word_t::operator<<(const word_t& other) const {
    return word_t(this->u << other.u);
}

word_t& word_t::operator<<=(const word_t& other) {
    return *this = *this << other;
}

word_t word_t::srl(const word_t& other) const {
    return word_t(this->u >> other.u);
}

word_t word_t::sra(const word_t& other) const {
    //TODO ensure this is arithmetic shift
    return word_t(this->s >> other.s);
}

bool word_t::operator!() const {
    return this->u == 0;
}

bool word_t::operator==(const word_t& other) const {
    return this->u == other.u;
}

bool word_t::operator!=(const word_t& other) const {
    return !(*this == other);
}

word_t word_t::bit(uint8_t bit) const {
    return this->bits(bit, bit);
}

word_t word_t::bits(uint8_t top_bit, uint8_t bottom_bit) const {
    assert((top_bit >= bottom_bit) && "Bad arguments to bits()");
    assert((top_bit < 32) && "Bad arguments to bits()");
    assert((bottom_bit < 32) && "Bad arguments to bits()");

    //Move the lowest bit desired to the bit 0 position
    word_t intermediate = this->srl(bottom_bit);
    
    //Generate the mask
    uint8_t num_bits = top_bit - bottom_bit + 1;
    word_t mask = (1 << num_bits) - 1;

    //Apply the mask and return
    return intermediate & mask;
}

word_t word_t::sign_extend_upward_from_bit(uint8_t bit) const {//Sign extend from a bit upward 
    assert((bit < 32) && "Bad argument to sign_extend_from()");

    uint8_t shift_amount = 31 - bit;

    //Shift the topmost bit to be extended to the top of the word
    word_t intermediate = *this << shift_amount;

    //ARITHMETIC shift things back down to perform sign extension and return
    return intermediate.sra(shift_amount);

}
