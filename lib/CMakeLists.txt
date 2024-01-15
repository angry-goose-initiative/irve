# CMakeLists.txt
# Copyright (C) 2023-2024 John Jekel
# See the LICENSE file at the root of the project for licensing info.
#
# CMake build file for rvsw/lib
#
# Based on CMakeLists.txt from irve
#

cmake_minimum_required(VERSION 3.16.3)

add_subdirectory(c)
add_subdirectory(smode_newlib_syscalls)
add_subdirectory(cxx)
