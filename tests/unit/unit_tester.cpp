/* unit_tester.cpp
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Performs unit tests for IRVE 
 *
*/

//FIXME when in release mode, assertions in libirve will be disabled. But we want to leave assertions actual unit tests enabled!

/* Test List */

#define TEST_LIST \
    X(test_word_t) \
    X(test_integer_pow) \
    X(test_cpu_state_t) \
    X(test_decoded_inst_t) \
    X(test_decoded_inst_t_invalid) \
    X(test_reg_file_t) \
    X(test_memory_t_valid) \
    X(test_memory_t_invalid) \
    X(test_pmemory_t_valid) \
    X(test_pmemory_t_invalid)
    //TODO add more

/* Includes */

#include <iostream>
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <string>
#include <unordered_map>

/* External Function Declarations */

#define X(test_name) extern int test_name();
TEST_LIST
#undef X

/* Constants And Defines */

#define X(test_name) {#test_name, test_name},
const std::unordered_map<std::string, int (*)()> TEST_MAP = {
    TEST_LIST
};
#undef X

/* Function Implementations */

int main(int argc, char** argv) {
    assert(argc == 2 && "Incorrect number of arguments. Usage: unit_tester test_to_run");

    //Initialize random number generator for tests that use it
    srand(time(NULL));
    
    //Lookup the test to run and run it
    auto test = TEST_MAP.find(argv[1]);
    assert((test != TEST_MAP.end()) && "Test not found. Check the spelling and that it is in TEST_LIST");
    std::cout << "Running test: \"" << argv[1] << "\"" << std::endl;
    return test->second();
}
