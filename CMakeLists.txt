# CMakeLists.txt
# Copyright (C) 2023-2024 John Jekel
# See the LICENSE file at the root of the project for licensing info.
#
# CMake configuration file for irve project
#
# Based on CMakeLists.txt from rv32esim
#

#Config options

#TODO when IRVE is complete, make these suitable for a release build by default

#TODO add option to disable instruction cache

#Default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
    #set(CMAKE_BUILD_TYPE Release)
endif()

#Disable verbose logging
if(NOT DEFINED IRVE_DISABLE_LOGGING)
    #set(IRVE_DISABLE_LOGGING 0)#TODO rename to disable verbose logging
    set(IRVE_DISABLE_LOGGING 1)#TODO rename to disable verbose logging
endif()

#Do logging on a separate thread
if(NOT DEFINED IRVE_ASYNC_LOGGING)
    set(IRVE_ASYNC_LOGGING 0)
    #set(IRVE_ASYNC_LOGGING 1)
endif()

#Optional to use most things, but notably required for pretty disassembly
if(NOT DEFINED IRVE_RUST)
    set(IRVE_RUST 0)
    #set(IRVE_RUST 1)
endif()

#Enable ASAN, UBSAN, etc
if(NOT DEFINED IRVE_SANITIZE)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        if (IRVE_RUST)#TODO remove this check once irvers issues are resolved
            set(IRVE_SANITIZE 0)
        else()
            set(IRVE_SANITIZE 1)
        endif()
    else()
        set(IRVE_SANITIZE 0)
    endif()
endif()

#If you want a collection of RISC-V software to mess with
if(NOT DEFINED IRVE_USE_RVSW)
    #set(IRVE_USE_RVSW 0)
    set(IRVE_USE_RVSW 1)
endif()

#Enable pseudo-fuzzing
if(NOT DEFINED IRVE_FUZZISH)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(IRVE_FUZZISH 1)
    else()
        set(IRVE_FUZZISH 0)
    endif()
endif()

#Enable Inception mode: cross-compile the emulator itself for RISC-V
if(NOT DEFINED IRVE_INCEPTION)
    set(IRVE_INCEPTION 0)
    #set(IRVE_INCEPTION 1)
endif()

#################################################################################################################################

if (IRVE_INCEPTION)
    set(CMAKE_CXX_COMPILER riscv32-unknown-linux-gnu-g++)
endif()

cmake_minimum_required(VERSION 3.16.3)
project(irve VERSION 1.0.0)
#Needed for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(CTest)

#https://stackoverflow.com/questions/41361631/optimize-in-cmake-by-default

set(GIT_EXECUTABLE git)

set(CMAKE_CXX_FLAGS "-std=c++2b -Wall -Wextra -Werror -Wno-error=cpp")

if (IRVE_INCEPTION)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")
endif()

if (IRVE_SANITIZE)
    if (NOT APPLE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=pointer-compare -fsanitize=pointer-subtract -fsanitize=leak -fsanitize=undefined")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=pointer-compare -fsanitize=pointer-subtract -fsanitize=undefined")
    endif()
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb3")
if (IRVE_RUST)
    set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3")#FIXME irvers dosn't like to compile with lto enabled; though regular irve and irve_disassemble are fine
else()
    if (NOT APPLE)
        set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3 -flto=auto -fuse-linker-plugin")
    else()
        set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3 -flto=auto")
    endif()
endif()

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
