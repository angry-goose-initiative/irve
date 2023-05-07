/* unit_tester.cpp
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Performs unit tests for IRVE 
 *
*/

/* Test List */

#define TEST_LIST \
    X(test_word_t) \
    X(test_integer_pow)
    //TODO add more

/* Includes */

#include <iostream>
#include <cassert>
#include <string>
#include <unordered_map>

/* External Function Declarations */

#define X(test) extern int test();
TEST_LIST
#undef X

/* Constants And Defines */

#define X(test) {#test, test},
const std::unordered_map<std::string, int (*)()> TEST_MAP = {
    TEST_LIST
};
#undef X

/* Function Implementations */

int main(int argc, char** argv) {
    assert(argc == 2 && "Incorrect number of arguments. Usage: unit_tester test_to_run");
    
    //Lookup the test to run and run it
    auto test = TEST_MAP.find(argv[1]);
    assert((test != TEST_MAP.end()) && "Test not found. Check the spelling and that it is in TESTS");
    std::cout << "Running test: \"" << argv[1] << "\"" << std::endl;
    return test->second();
}
