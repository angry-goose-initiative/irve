# CMakeLists.txt
# Copyright (C) 2023 John Jekel and Nick Chan
# See the LICENSE file at the root of the project for licensing info.
#
# CMake configuration file for irve project
#
# Based on CMakeLists.txt from rv32esim
#

#Common options
cmake_minimum_required(VERSION 3.16.3)
project(irve VERSION 0.2.1)
#Needed for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(CTest)

#https://stackoverflow.com/questions/41361631/optimize-in-cmake-by-default
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(GIT_EXECUTABLE git)

set(CMAKE_CXX_FLAGS "-Wall -std=c++20")
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -fsanitize=address")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto=auto -fuse-linker-plugin")

set(IRVE_DISABLE_LOGGING 0)

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
add_subdirectory(testfiles)
add_subdirectory(lib)
add_subdirectory(src)
