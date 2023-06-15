# CMakeLists.txt
# Copyright (C) 2023 John Jekel and Nick Chan
# See the LICENSE file at the root of the project for licensing info.
#
# CMake build file for rvsw
#
# Based on CMakeLists.txt from rv32esim
#

#Common options
cmake_minimum_required(VERSION 3.16.3)
project(rvsw VERSION 0.0.0 LANGUAGES C ASM)
#Needed for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (NOT RVSW_CONFIG_PATH)
    message(FATAL_ERROR "The CMake variable RVSW_CONFIG_PATH must be set to the path of the rvsw config file. Bailing out.")
else()
    message("Using the rvsw config file at ${RVSW_CONFIG_PATH}")
    include(${RVSW_CONFIG_PATH})
endif()

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

#add_subdirectory(firm)
#add_subdirectory(lib)
#add_subdirectory(sbi)
#add_subdirectory(src)
