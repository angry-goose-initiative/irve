/* common.cpp
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Performs unit tests for IRVE's common.h and common.cpp
 *
*/

/* Includes */

#include <cassert>
#include "common.h"

/* Function Implementations */

int test_word_t() {
    assert(word_t(1) + word_t(1) == word_t(2));
    assert(word_t(1) - word_t(-1) == word_t(2));
    assert(word_t(-1) == word_t(0xFFFFFFFF));
    assert(word_t(1) * word_t(-1) == word_t(0xFFFFFFFF));
    assert(word_t(0x80000000).srl(4) == word_t(0x08000000));
    assert(word_t(0x80000000).sra(4) == word_t(0xF8000000));

    //TODO add more
    return 0;
}

int test_integer_pow() {
    assert(upow(2, 0) == 1);
    assert(upow(2, 1) == 2);
    assert(upow(2, 2) == 4);
    assert(upow(2, 3) == 8);

    //TODO add more
    return 0;
}
