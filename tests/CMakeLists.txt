# CMakeLists.txt
# Copyright (C) 2023 John Jekel and Nick Chan
# See the LICENSE file at the root of the project for licensing info.
#
# CMake configuration file for irve tests
#
# NOTE: Unlike the testfiles directory this is NOT a seperate project
#
# Based on CMakeLists.txt from rv32esim
#

#Common options
cmake_minimum_required(VERSION 3.16.3)
#Needed for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(CTest)

#To get access to libirve headers
include_directories(${PROJECT_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src)

#https://stackoverflow.com/questions/41361631/optimize-in-cmake-by-default
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_FLAGS "-Wall -std=c++20")
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -fsanitize=address")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto=auto -fuse-linker-plugin")

set(UNIT_TESTER_SOURCES
    unit/unit_tester.cpp
    unit/common.cpp
    unit/cpu_state.cpp
    unit/decode.cpp
    unit/memory.cpp
    unit/reg_file.cpp
)

set(INTEGRATION_TESTER_SOURCES
    integration/integration_tester.cpp
)

if (CMAKE_C_BYTE_ORDER STREQUAL LITTLE_ENDIAN)
    set(IS_LITTLE_ENDIAN 1)
else()
    set(IS_LITTLE_ENDIAN 0)
endif()

add_executable(unit_tester ${UNIT_TESTER_SOURCES})
target_link_libraries(unit_tester PRIVATE libirve_static)#TODO or should we make this shared?

add_executable(integration_tester ${INTEGRATION_TESTER_SOURCES})
target_link_libraries(integration_tester PRIVATE libirve_static)#TODO or should we make this shared?

#########################################################################################
# Unit tests
#########################################################################################
macro(add_unit_test TEST_NAME)
    add_test(NAME unit_${TEST_NAME} COMMAND unit_tester test_${TEST_NAME})
endmacro()

add_unit_test(word_t)
add_unit_test(integer_pow)
add_unit_test(cpu_state_t)
add_unit_test(decoded_inst_t)
add_unit_test(decoded_inst_t_invalid)
add_unit_test(reg_file_t)
add_unit_test(memory_t_valid)
add_unit_test(memory_t_invalid)
add_unit_test(pmemory_t_valid)
add_unit_test(pmemory_t_invalid)

#########################################################################################
# Integration tests
#########################################################################################
macro(add_integration_test TEST_NAME)
    add_test(NAME integration_${TEST_NAME} COMMAND integration_tester test_${TEST_NAME})
endmacro()

#TODO the integration tester will test for assertions both in IRVE and RISC-V code
#It will have an internal list of tests to run which will include the path to
#the testfile, and a function to verify that the CPU state, memory, and output
#were as expected
