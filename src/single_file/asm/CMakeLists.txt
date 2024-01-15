# CMakeLists.txt
# Copyright (C) 2023-2024 John Jekel
# See the LICENSE file at the root of the project for licensing info.
#
# CMake configuration file for single file assembly tests
#
# Based on testfiles/CMakeLists.txt from irve
#

#Common options
cmake_minimum_required(VERSION 3.16.3)

include(${PROJECT_SOURCE_DIR}/macros.cmake)

macro(add_asm_target NAME)
    add_target(${NAME} ${ARGN})
    target_include_directories(src__single_file__asm__${NAME} PRIVATE ${PROJECT_BINARY_DIR}/include)#To get the generated rvsw_asm.h
endmacro()

add_asm_target(rv32esim rv32esim.S)
add_asm_target(privilege_levels privilege_levels.S)
add_asm_target(user_and_back_in_time_for_dinner user_and_back_in_time_for_dinner.S)
add_asm_target(atomics atomics.S)

add_subdirectory(jzjcoresoftware)
