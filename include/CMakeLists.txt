# CMakeLists.txt
# Copyright (C) 2023-2024 John Jekel
# See the LICENSE file at the root of the project for licensing info.
#
# CMake build file for rvsw/src
#
# Based on CMakeLists.txt from rv32esim
#

cmake_minimum_required(VERSION 3.16.3)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/rvsw_asm.h.in ${CMAKE_CURRENT_BINARY_DIR}/rvsw_asm.h @ONLY)
