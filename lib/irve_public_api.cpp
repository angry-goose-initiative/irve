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
#include "emulator.h"

#define INST_COUNT 0
#include "logging.h"

//NO using statements here to make it obvious if we are refering to the internal namespace or the public namespace

/* Function Implementations */

//TODO expose emulator_t

/*void irve::loader::load_verilog_32(irve::emulator_t& emulator, const char* filename) {
    irve::internal::loader::load_verilog_32(emulator, filename);
}*/

void irve::logging::log(uint8_t indent, const char* str, ...) {
#if not(IRVE_INTERNAL_CONFIG_DISABLE_LOGGING)
    va_list list;
    va_start(list, str);

    irvelog(indent, str, list);
#endif
}

bool irve::logging::logging_disabled() {
    return IRVE_INTERNAL_CONFIG_DISABLE_LOGGING;
}

std::size_t irve::about::get_version_major() {
    return IRVE_INTERNAL_CONFIG_VERSION_MAJOR;
}

std::size_t irve::about::get_version_minor() {
    return IRVE_INTERNAL_CONFIG_VERSION_MINOR;
}

std::size_t irve::about::get_version_patch() {
    return IRVE_INTERNAL_CONFIG_VERSION_PATCH;
}

const char* irve::about::get_version_string() {
    return IRVE_INTERNAL_CONFIG_VERSION_STRING;
}

const char* irve::about::get_build_time_string() {
    return IRVE_INTERNAL_CONFIG_BUILD_TIME_STRING;
}

const char* irve::about::get_build_date_string() {
    return IRVE_INTERNAL_CONFIG_BUILD_DATE_STRING;
}

