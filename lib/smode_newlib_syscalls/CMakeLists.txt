# CMakeLists.txt
# Copyright (C) 2023-2024 John Jekel
# See the LICENSE file at the root of the project for licensing info.
#
# CMake configuration file for RVSW common c code
#
# Based on testfiles/CMakeLists.txt from irve
#

#Common options
cmake_minimum_required(VERSION 3.16.3)

#include(${PROJECT_SOURCE_DIR}/macros.cmake)

set(RVSW_SMODE_NEWLIB_SYSCALLS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/smode_newlib_syscalls.c
)

add_library(rvsw_smode_newlib_syscalls OBJECT ${RVSW_SMODE_NEWLIB_SYSCALLS_SOURCES})#FIXME why won't this work with STATIC?
target_include_directories(rvsw_smode_newlib_syscalls PRIVATE ${PROJECT_BINARY_DIR}/include)#To get the generated rvsw_asm.h

#Add S-Mode specific flags
target_compile_options(rvsw_smode_newlib_syscalls PRIVATE ${RVSW_SMODE_COMMON_FLAGS})
target_link_options(rvsw_smode_newlib_syscalls PRIVATE ${RVSW_SMODE_COMMON_FLAGS})
