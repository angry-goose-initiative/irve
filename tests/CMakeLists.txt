# CMakeLists.txt
# Copyright (C) 2023 John Jekel and Nick Chan
# See the LICENSE file at the root of the project for licensing info.
#
# CMake configuration file for irve tests
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

set(CMAKE_CXX_FLAGS "-Wall -Wextra -std=c++20")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -fsanitize=address")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto=auto -fuse-linker-plugin")

set(UNIT_TESTER_SOURCES unit/common.cpp unit/unit_tester.cpp)

if (CMAKE_C_BYTE_ORDER STREQUAL LITTLE_ENDIAN)
    set(IS_LITTLE_ENDIAN 1)
else()
    set(IS_LITTLE_ENDIAN 0)
endif()

add_executable(unit_tester ${UNIT_TESTER_SOURCES})
target_link_libraries(unit_tester PRIVATE libirve_static)#TODO or should we make this shared?

#########################################################################################
# Unit tests
#########################################################################################
macro(add_unit_test TEST_NAME)
    add_test(NAME ${TEST_NAME} COMMAND unit_tester ${TEST_NAME})
endmacro()

add_unit_test(test_word_t)
add_unit_test(test_integer_pow)
add_unit_test(test_intentionally_fail)
