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
#include "loader.h"

#define INST_COUNT 0
#include "logging.h"

//NO using statements here to make it obvious if we are refering to the internal namespace or the public namespace

/* Function Implementations */

//Namepace: irve::emulator

irve::emulator::emulator_t::emulator_t() : m_emulator_ptr(new irve::internal::emulator::emulator_t()) {}

irve::emulator::emulator_t::~emulator_t() {
    delete this->m_emulator_ptr;
    this->m_emulator_ptr = nullptr;
}

bool irve::emulator::emulator_t::tick() {
    return this->m_emulator_ptr->tick();
}

void irve::emulator::emulator_t::run_until(uint64_t inst_count) {
    this->m_emulator_ptr->run_until(inst_count);
}

uint64_t irve::emulator::emulator_t::get_inst_count() const {
    return this->m_emulator_ptr->get_inst_count();
}

uint8_t irve::emulator::emulator_t::mem_read_byte(uint32_t addr) const {
    return (uint8_t)this->m_emulator_ptr->mem_read_byte(addr);
}

void irve::emulator::emulator_t::mem_write_byte(uint32_t addr, uint8_t data) {
    this->m_emulator_ptr->mem_write(addr, 0b000, (uint32_t)data);//FIXME why isn't this orthogonal in the internal API?
}

//Namepace: irve::loader

void irve::loader::load_verilog_32(irve::emulator::emulator_t& emulator, const char* filename) {
    irve::internal::loader::load_verilog_32(*(emulator.m_emulator_ptr), filename);
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
    irvelog_raw(indent, buffer);

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
    irvelog_raw_always(indent, buffer);

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
