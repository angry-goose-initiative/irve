/* common.cpp
 * Copyright (C) TODO John Jekel
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Function implementations for common.h
 *
*/

/* Constants And Defines */

//TODO

/* Includes */

#include "common.h"

/* Types */

//TODO

/* Variables */

//TODO

/* Static Function Declarations */

//TODO

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

word_t word_t::operator-(const word_t& other) const {
    //TODO ensure this is the same for signed
    return word_t(this->u - other.u);
}

word_t word_t::operator*(const word_t& other) const {
    return word_t(this->u * other.u);
}

word_t word_t::operator~() const {
    return word_t(~this->u);
}

word_t word_t::operator|(const word_t& other) const {
    return word_t(this->u | other.u);
}

word_t word_t::operator&(const word_t& other) const {
    return word_t(this->u & other.u);
}

word_t word_t::operator^(const word_t& other) const {
    return word_t(this->u ^ other.u);
}

word_t word_t::operator<<(const word_t& other) const {
    return word_t(this->u << other.u);
}

word_t word_t::srl(const word_t& other) const {
    return word_t(this->u >> other.u);
}

word_t word_t::sra(const word_t& other) const {
    //TODO ensure this is arithmetic shift
    return word_t(this->s >> other.u);
}

/* Static Function Implementations */

//TODO
