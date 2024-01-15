# CMakeLists.txt
# Copyright (C) 2023-2024 John Jekel
# See the LICENSE file at the root of the project for licensing info.
#
# CMake configuration file for single_file
#
# Based on testfiles/CMakeLists.txt from irve
#

#Common options
cmake_minimum_required(VERSION 3.16.3)

add_subdirectory("asm")
add_subdirectory("c")
add_subdirectory("cxx")
