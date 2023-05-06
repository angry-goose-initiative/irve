# CMakeLists.txt
# Copyright (C) 2023 John Jekel and Nick Chan
# See the LICENSE file at the root of the project for licensing info.
#
# CMake configuration file for irve
#
# Based on CMakeLists.txt from rv32esim
#

#Common options
cmake_minimum_required(VERSION 3.21)
project(irve VERSION 0.1)
#Needed for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_subdirectory(testfiles)

#https://stackoverflow.com/questions/41361631/optimize-in-cmake-by-default
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra -std=c++20")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -fsanitize=address")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto=auto -fuse-linker-plugin")

#set(RV32ESIM_UNSAFE 0)#Speed boost; may cause out of bounds/etc problems if programs executing on the simulator go out-of-bounds/etc
#set(RV32ESIM_DISABLE_LOGGING 0)#Speed boost (avoid checking if enabled, allows compiler to optimize some things more, etc)
#set(RV32ESIM_C 1)#Support compressed extension

set(IRVE_SOURCES
    src/cmake_config.h src/common.h src/cpu_state.cpp src/cpu_state.h
    src/decode.cpp src/decode.h src/emulator.cpp src/emulator.h
    src/execute.cpp src/execute.h src/logging.cpp src/logging.h
    src/main.cpp src/memory.cpp src/memory.h src/reg_file.cpp
    src/reg_file.h src/reg.h
)

if (CMAKE_C_BYTE_ORDER STREQUAL LITTLE_ENDIAN)
    set(IS_LITTLE_ENDIAN 1)
else()
    set(IS_LITTLE_ENDIAN 0)
endif()

configure_file(src/cmake_config.h.in src/cmake_config.h)

add_executable(irve ${IRVE_SOURCES})
