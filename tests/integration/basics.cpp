/* basics.cpp
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Performs integration tests to ensure that all the trivial functions
 * (ex. in the irve::about namespace, logging, etc.) work as expected
 *
*/

/* Includes */

#include "irve_public_api.h"

#include <iostream>

/* Function Implementations */

int test_basics() {//None of these functions should crash or throw exceptions
    std::cerr << "libirve " << irve::about::get_version_string() << std::endl;
    std::cerr << "libirve built at " << irve::about::get_build_time_string() << " on " << irve::about::get_build_date_string() << std::endl;
    std::cerr << "Build system: " << irve::about::get_build_system_string() << std::endl;
    std::cerr << "Build from " << irve::about::get_build_host_string() << " for " << irve::about::get_compile_target_string() << std::endl;
    std::cerr << "Compiler: " << irve::about::get_compiler_string() << std::endl;
    std::cerr << "Logging is " << (irve::logging::logging_disabled() ? "disabled" : "enabled") << std::endl;

    for (int i = -1; i < 123; i++) {
        irve::logging::log(i, "This is a test log message");
        irve::logging::log(i + 1, "This is a message with a number: %d", 42);
        irve::logging::log(i - 1, "Negative indent hopefully won't crash %f", 3.14159);
        irve::logging::log(i, "This is a message with a string: %s", "Hello, world!");
        irve::logging::log(i, "This is a message with a char: %c", 'A');
        irve::logging::log(i, "This is a message with a pointer: %p", test_basics);
    }
    
    return 0;
}
