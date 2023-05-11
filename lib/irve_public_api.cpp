/* irve_public_api.cpp
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Implementation of the public API for IRVE (sort of a ABI-compatibility wrapper)
 * 
*/

/* Includes */

#include "irve_public_api.h"
#include "cmake_config.h"

#include <cstddef>
#include <cstdint>

#define INST_COUNT 0
#include "logging.h"

/* Function Implementations */

//TODO expose emulator_t

void irve::log(uint8_t indent, const char* str, ...) {
#if not(CMAKE_IRVE_DISABLE_LOGGING)
    va_list list;
    va_start(list, str);

    irvelog(indent, str, list);
#endif
}

std::size_t irve::about::get_version_major() {
    return CMAKE_IRVE_VERSION_MAJOR;
}

std::size_t irve::about::get_version_minor() {
    return CMAKE_IRVE_VERSION_MINOR;
}

std::size_t irve::about::get_version_patch() {
    return CMAKE_IRVE_VERSION_PATCH;
}

const char* irve::about::get_version_string() {
    return CMAKE_IRVE_VERSION_STRING;
}

bool irve::about::logging_disabled() {
    return CMAKE_IRVE_DISABLE_LOGGING;
}
