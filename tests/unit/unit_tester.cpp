/* unit_tester.cpp
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Performs unit tests for IRVE 
 *
*/

/* Includes */

#include <unordered_map>
#include <string>

//TESTING move these to a separate file
#include <cassert>
#include "common.h"
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

/* Constants And Defines */

const std::unordered_map<std::string, int (*)()> TESTS = {
    {"test_word_t", test_word_t},
    {"test_integer_pow", test_integer_pow},//TODO
};

/* Function Implementations */

int main(int argc, char** argv) {
    assert(argc == 2 && "Incorrect number of arguments. Usage: unit_tester test_to_run");
    
    //Lookup the test to run and run it
    auto test = TESTS.find(argv[1]);
    assert((test != TESTS.end()) && "Test not found. Check the spelling and that it is in TESTS");
    return test->second();
}
