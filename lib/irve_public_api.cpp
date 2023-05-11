/* irve_public_api.cpp
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Implementation of the public API for IRVE (sort of a ABI-compatibility wrapper)
 * 
*/

/* Includes */

#include "irve_public_api.h"

#include <cstddef>
#include <cstdint>

#include "config.h"

#define INST_COUNT 0
#include "logging.h"

using namespace irve;

/* Function Implementations */

//TODO expose emulator_t

void irve::log(uint8_t indent, const char* str, ...) {
#if not(IRVE_CONFIG_DISABLE_LOGGING)
    va_list list;
    va_start(list, str);

    irvelog(indent, str, list);
#endif
}

std::size_t about::get_version_major() {
    return IRVE_CONFIG_VERSION_MAJOR;
}

std::size_t about::get_version_minor() {
    return IRVE_CONFIG_VERSION_MINOR;
}

std::size_t about::get_version_patch() {
    return IRVE_CONFIG_VERSION_PATCH;
}

const char* about::get_version_string() {
    return IRVE_CONFIG_VERSION_STRING;
}

bool about::logging_disabled() {
    return IRVE_CONFIG_DISABLE_LOGGING;
}
