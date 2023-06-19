# CMakeLists.txt
# Copyright (C) 2023 John Jekel and Nick Chan
# See the LICENSE file at the root of the project for licensing info.
#
# CMake configuration file for irve project
#
# Based on CMakeLists.txt from rv32esim
#

#Config options
set(IRVE_DISABLE_LOGGING 0)#TODO make this enabled by default in release builds#TODO rename to disable verbose logging
set(IRVE_ASYNC_LOGGING 0)
set(IRVE_SANITIZE 1)
set(IRVE_RUST 0)#Optional to use most things, but notably required for pretty disassembly
set(IRVE_USE_RVSW 1)#If you want a collection of RISC-V software to mess with

#################################################################################################################################

cmake_minimum_required(VERSION 3.16.3)
project(irve VERSION 0.5.0)
#Needed for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(CTest)

#https://stackoverflow.com/questions/41361631/optimize-in-cmake-by-default
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(GIT_EXECUTABLE git)

set(CMAKE_CXX_FLAGS "")
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++20 /Wall /WX")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -Wall -Wextra -Werror")
endif()

if (IRVE_SANITIZE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=pointer-compare -fsanitize=pointer-subtract -fsanitize=leak -fsanitize=undefined")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto=auto -fuse-linker-plugin")

#https://stackoverflow.com/questions/1435953/how-can-i-pass-git-sha1-to-compiler-as-definition-using-cmake
execute_process(
    COMMAND                             "${GIT_EXECUTABLE}" describe --match=NeVeRmAtCh --always --abbrev=40 --dirty
    WORKING_DIRECTORY                   "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE                     IRVE_GIT_CHECKSUM
    ERROR_QUIET
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

#So these all get the variables set here, put them at the end
add_subdirectory(tests)
add_subdirectory(lib)
add_subdirectory(src)
if (IRVE_USE_RVSW)
    add_subdirectory(rvsw)
endif()
