/**
 * @brief   Implementation of the public API for IRVE (sort of a ABI-compatibility wrapper)
 * 
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  Copyright (C) 2023 Nick Chan\n
 *  See the LICENSE file at the root of the project for licensing info.
 * 
 * TODO longer description
 *
*/

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include "irve_public_api.h"

#include <cstddef>
#include <cstdint>

#include "config.h"
#include "emulator.h"

#define INST_COUNT 0
#include "logging.h"

//NO using statements here to make it obvious if we are refering to the internal namespace or the
//public namespace

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

//Namepace: irve::emulator

irve::Emulator::Emulator(int imagec, const char* const* imagev):
    m_emulator_ptr(new irve::internal::Emulator(imagec, imagev)) {}

irve::Emulator::~Emulator() {
    delete this->m_emulator_ptr;
    this->m_emulator_ptr = nullptr;
}

bool irve::Emulator::tick() {
    return this->m_emulator_ptr->tick();
}

void irve::Emulator::run_until(uint64_t inst_count) {
    this->m_emulator_ptr->run_until(inst_count);
}

void irve::Emulator::run_gdbserver(uint16_t port) {
    this->m_emulator_ptr->run_gdbserver(port);
}

uint64_t irve::Emulator::get_inst_count() const {
    return this->m_emulator_ptr->get_inst_count();
}

//Namepace: irve::logging

#if IRVE_INTERNAL_CONFIG_DISABLE_LOGGING

void irve::logging::log(uint8_t, const char*, ...) {}

#else

void irve::logging::log(uint8_t indent, const char* str, ...) {
    va_list list_copy_1;
    va_start(list_copy_1, str);
    va_list list_copy_2;
    va_copy(list_copy_2, list_copy_1);

    //Perform variable argument string formatting into a buffer
    std::size_t buffer_size = std::vsnprintf(nullptr, 0, str, list_copy_1) + 1;//vsnprintf doesn't count the null terminator
    va_end(list_copy_1);
    char* buffer = (char*)std::malloc(buffer_size);
    std::vsnprintf(buffer, buffer_size, str, list_copy_2);
    va_end(list_copy_2);

    //Actually log the string
    irvelog(indent, buffer);

    //Free the buffer
    std::free(buffer);
}

#endif

void irve::logging::log_always(uint8_t indent, const char* str, ...) {//USE THIS SPARINGLY
    va_list list_copy_1;
    va_start(list_copy_1, str);
    va_list list_copy_2;
    va_copy(list_copy_2, list_copy_1);

    //Perform variable argument string formatting into a buffer
    std::size_t buffer_size = std::vsnprintf(nullptr, 0, str, list_copy_1) + 1;//vsnprintf doesn't count the null terminator
    va_end(list_copy_1);
    char* buffer = (char*)std::malloc(buffer_size);
    std::vsnprintf(buffer, buffer_size, str, list_copy_2);
    va_end(list_copy_2);

    //Actually log the string
    irvelog_always(indent, buffer);

    //Free the buffer
    std::free(buffer);
}

bool irve::logging::logging_disabled() {
    return IRVE_INTERNAL_CONFIG_DISABLE_LOGGING;
}

//Namepace: irve::about

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

const char* irve::about::get_build_system_string() {
    return IRVE_INTERNAL_CONFIG_BUILD_SYSTEM_STRING;
}

const char* irve::about::get_build_host_string() {
    return IRVE_INTERNAL_CONFIG_BUILD_HOST_STRING;
}

const char* irve::about::get_compile_target_string() {
    return IRVE_INTERNAL_CONFIG_COMPILE_TARGET_STRING;
}

const char* irve::about::get_compiler_string() {
    return IRVE_INTERNAL_CONFIG_COMPILER_STRING;
}

bool irve::about::fuzzish_build() {
    return IRVE_INTERNAL_CONFIG_FUZZISH == 1;
}
