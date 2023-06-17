# CMakeLists.txt
# Copyright (C) 2023 John Jekel and Nick Chan
# See the LICENSE file at the root of the project for licensing info.
#
# CMake configuration file for RVSW common c code
#
# Based on testfiles/CMakeLists.txt from irve
#

#Common options
cmake_minimum_required(VERSION 3.16.3)

#include(${PROJECT_SOURCE_DIR}/macros.cmake)

#macro(add_asm_target NAME)
#add_target(${NAME} ${ARGN})
#target_include_directories(src__single_file__asm__${NAME} PRIVATE ${PROJECT_BINARY_DIR}/src)#To get the generated rvsw_asm.h
#endmacro()

#Assembly test files from rv32esim
#add_asm_target(rv32esim rv32esim.S)

#TODO others

#add_subdirectory(jzjcoresoftware)
