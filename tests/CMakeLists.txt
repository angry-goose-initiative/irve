# CMakeLists.txt
# Copyright (C) 2023-2024 John Jekel
# See the LICENSE file at the root of the project for licensing info.
#
# CMake configuration file for irve tests
#
# Based on CMakeLists.txt from rv32esim
#

#Common options
cmake_minimum_required(VERSION 3.16.3)
include(CTest)
add_subdirectory(unit)
add_subdirectory(integration)
if (IRVE_USE_RVSW)
    add_subdirectory(rvsw)
endif()
add_subdirectory(riscv_arch_test)
