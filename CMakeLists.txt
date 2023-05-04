# crt0.s
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

#https://stackoverflow.com/questions/41361631/optimize-in-cmake-by-default
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

#-I is not needed to compile but is needed for clangd to find the include directory
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -fsanitize=address")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto=auto -fuse-linker-plugin")

#rv32esim Library Targets
#set(RV32ESIM_UNSAFE 0)#Speed boost; may cause out of bounds/etc problems if programs executing on the simulator go out-of-bounds/etc
#set(RV32ESIM_DISABLE_LOGGING 0)#Speed boost (avoid checking if enabled, allows compiler to optimize some things more, etc)
#set(RV32ESIM_C 1)#Support compressed extension

#set(IRVE_SOURCES src/

if (CMAKE_C_BYTE_ORDER STREQUAL LITTLE_ENDIAN)
    set(IS_LITTLE_ENDIAN 1)
else()
    set(IS_LITTLE_ENDIAN 0)
endif()

configure_file(src/rv32esim/cmake_config.h.in src/rv32esim/cmake_config.h)

add_executable(irve.bin ${IRVE_SOURCES})

#add_library(rv32esim_shared SHARED ${RV32ESIM_SOURCES})
#add_library(rv32esim_static STATIC ${RV32ESIM_SOURCES})
#target_include_directories(rv32esim_shared PRIVATE include ${CMAKE_CURRENT_BINARY_DIR}/src/rv32esim/)
#target_include_directories(rv32esim_static PRIVATE include ${CMAKE_CURRENT_BINARY_DIR}/src/rv32esim/)
#set_property(TARGET rv32esim_shared PROPERTY OUTPUT_NAME rv32esim)
#set_property(TARGET rv32esim_static PROPERTY OUTPUT_NAME rv32esim)

#rv32esim Test Code
#set(TEST_SOURCES src/main.c include/rv32esim.h)
#add_executable(rv32esim ${TEST_SOURCES})
#target_include_directories(rv32esim PRIVATE include)
#target_link_libraries(rv32esim rv32esim_shared)
