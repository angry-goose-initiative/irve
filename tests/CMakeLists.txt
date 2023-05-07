# CMakeLists.txt
# Copyright (C) 2023 John Jekel and Nick Chan
# See the LICENSE file at the root of the project for licensing info.
#
# CMake configuration file for irve tests
#
# Based on CMakeLists.txt from rv32esim
#

#Common options
cmake_minimum_required(VERSION 3.21)
project(irve_tests VERSION 0.1.0)
#Needed for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(CTest)

include_directories(../src ${CMAKE_CURRENT_BINARY_DIR}/src)

#https://stackoverflow.com/questions/41361631/optimize-in-cmake-by-default
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra -std=c++20")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -fsanitize=address")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto=auto -fuse-linker-plugin")

set(IRVE_DISABLE_LOGGING 0)

set(IRVE_SOURCES_EXCEPT_MAIN
    ../src/cmake_config.h ../src/common.cpp ../src/common.h ../src/cpu_state.cpp
    ../src/cpu_state.h ../src/CSR.cpp ../src/CSR.h ../src/decode.cpp ../src/decode.h
    ../src/emulator.cpp ../src/emulator.h ../src/execute.cpp ../src/execute.h
    ../src/logging.cpp ../src/logging.h ../src/memory.cpp ../src/memory.h
    ../src/reg_file.cpp ../src/reg_file.h ../src/rvexception.cpp ../src/rvexception.h
)

if (CMAKE_C_BYTE_ORDER STREQUAL LITTLE_ENDIAN)
    set(IS_LITTLE_ENDIAN 1)
else()
    set(IS_LITTLE_ENDIAN 0)
endif()

add_executable(unit_tester unit/unit_tester.cpp ${IRVE_SOURCES_EXCEPT_MAIN})

#########################################################################################
# Unit tests
#########################################################################################
macro(add_unit_test TEST_NAME)
    add_test(NAME ${TEST_NAME} COMMAND unit_tester ${TEST_NAME})
endmacro()

add_unit_test(test_word_t)
add_unit_test(test_integer_pow)
