/**
 * @file    common.cpp
 * @brief   Performs unit tests for IRVE's common.h and common.cpp
 * 
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  See the LICENSE file at the root of the project for licensing info.
*/

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include <cmath>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include "common.h"

#include <stdexcept>

using namespace irve::internal;

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

int test_common_word_t() {
    assert(word_t(1) + word_t(1) == word_t(2));
    assert(word_t(1) - word_t(-1) == word_t(2));
    assert(word_t(-1) == word_t(0xFFFFFFFF));
    assert(word_t(1) * word_t(-1) == word_t(0xFFFFFFFF));
    assert(word_t(0x80000000).srl(4) == word_t(0x08000000));
    assert(word_t(0x80000000).sra(4) == word_t(0xF8000000));

    //TODO add more
    return 0;
}

int test_common_upow() {
    for (uint32_t i = 0; i <= 10000000; ++i) {
        for (uint32_t j = 0; j <= 10000; ++j) {
            double expected = std::pow(i, j);
            if (expected <= UINT32_MAX) {//So we don't test cases that overflow a uint32_t
                assert(upow(i, j) == (uint32_t)expected);
            } else {
                break;
            }
        }
    }

    return 0;
}

int test_common_ipow() {
    for (int32_t i = -1000000; i <= 1000000; i += 13) {//For speed, count by a prime number
        for (int32_t j = -100; j <= 100; j += 13) {//To get through cases a bit faster (much of the positive cases are already tested above anyways)
            if ((i == 0) && (j < 0)) {
                j = 0;
                continue;
            }

            double expected = std::pow(i, j);
            if ((expected <= INT32_MAX) && (expected >= INT32_MIN)) {//So we don't test cases that overflow an int32_t
                try {
                    assert(spow(i, j) == (int32_t)expected);
                } catch (const std::runtime_error&) {
                    //If we get an exception, it's probably because we overflowed and the denominator became 0
                    //In this case it's okay
                    assert(upow(i, j) == 0);
                }
            }
        }
    }

    return 0;
}
