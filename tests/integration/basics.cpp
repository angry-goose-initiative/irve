/**
 * @file    basics.cpp
 * @brief   Basic intergration tests
 * 
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  See the LICENSE file at the root of the project for licensing info.
 *
 * Performs integration tests to ensure that all the trivial functions
 * (ex. in the irve::about namespace, logging, etc.) work as expected
*/

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include "irve_public_api.h"

#include <iostream>

#undef NDEBUG//Asserts should work even in release mode for tests
#include <cassert>

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

int test_about() {
    //None of these functions should crash or throw exceptions
    std::cerr << "libirve " << irve::about::get_version_string() << std::endl;
    std::cerr << "libirve built at " << irve::about::get_build_time_string() << " on " << irve::about::get_build_date_string() << std::endl;
    std::cerr << "Build system: " << irve::about::get_build_system_string() << std::endl;
    std::cerr << "Build from " << irve::about::get_build_host_string() << " for " << irve::about::get_compile_target_string() << std::endl;
    std::cerr << "Compiler: " << irve::about::get_compiler_string() << std::endl;
    
    return 0;
}

int test_logging() {
    //None of these functions should crash or throw exceptions
    irve::logging::log(0, "Logging is %s", irve::logging::logging_disabled() ? "disabled" : "enabled"); 

    for (int i = -1; i < 123; i++) {
        irve::logging::log(i, "This is a test log message");
        irve::logging::log(i + 1, "This is a message with a number: %d", 42);
        irve::logging::log(i - 1, "Negative indent hopefully won't crash %f", 3.14159);
        irve::logging::log(i, "This is a message with a string: %s", "Hello, world!");
        irve::logging::log(i, "This is a message with a char: %c", 'A');
        irve::logging::log(i, "This is a message with a pointer: %p", test_logging);
        //TODO stress test it more?
    }

    return 0;
}
