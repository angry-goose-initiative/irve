/**
 * @file    logging.cpp
 * @brief   Performs unit tests for IRVE's logging.h and logging.cpp
 * 
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  See the LICENSE file at the root of the project for licensing info.
*/

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#undef NDEBUG//Asserts should work even in release mode for tests
#include <cassert>
#include <cstdint>

#define INST_COUNT i * 12345
#include "logging.h"

using namespace irve::internal;

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

int test_logging_irvelog() {
    for (int i = -1; i < 123; i++) {
        irvelog(i, "This is a test log message");
        irvelog(i, "This is a message with a number: %d", 42);
        irvelog(i, "Negative indent hopefully won't crash %f", 3.14159);
        irvelog(i, "This is a message with a string: %s", "Hello, world!");
        irvelog(i, "This is a message with a char: %c", 'A');
        irvelog(i, "This is a message with a pointer: %p", test_logging_irvelog);
        //TODO stress test it more?
    }

    return 0;
}

//TODO test the internal functions directly
