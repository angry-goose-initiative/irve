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
project(irve VERSION 0.1.0)
#Needed for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(CTest)
add_subdirectory(tests)
add_subdirectory(testfiles)

#TODO perhaps move the stuff below to a CMakeLists.txt in src?

#https://stackoverflow.com/questions/41361631/optimize-in-cmake-by-default
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra -std=c++20")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -fsanitize=address")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto=auto -fuse-linker-plugin")

set(IRVE_DISABLE_LOGGING 0)

set(IRVE_SOURCES ${PROJECT_SOURCE_DIR}/src/main.cpp)

set(LIBIRVE_SOURCES
    ${CMAKE_BINARY_DIR}/src/cmake_config.h
    ${PROJECT_SOURCE_DIR}/src/common.cpp
    ${PROJECT_SOURCE_DIR}/src/common.h
    ${PROJECT_SOURCE_DIR}/src/cpu_state.cpp
    ${PROJECT_SOURCE_DIR}/src/cpu_state.h
    ${PROJECT_SOURCE_DIR}/src/CSR.cpp
    ${PROJECT_SOURCE_DIR}/src/CSR.h
    ${PROJECT_SOURCE_DIR}/src/decode.cpp
    ${PROJECT_SOURCE_DIR}/src/decode.h
    ${PROJECT_SOURCE_DIR}/src/emulator.cpp
    ${PROJECT_SOURCE_DIR}/src/emulator.h
    ${PROJECT_SOURCE_DIR}/src/execute.cpp
    ${PROJECT_SOURCE_DIR}/src/execute.h
    ${PROJECT_SOURCE_DIR}/src/logging.cpp
    ${PROJECT_SOURCE_DIR}/src/logging.h
    ${PROJECT_SOURCE_DIR}/src/memory.cpp
    ${PROJECT_SOURCE_DIR}/src/memory.h
    ${PROJECT_SOURCE_DIR}/src/reg_file.cpp
    ${PROJECT_SOURCE_DIR}/src/reg_file.h
    ${PROJECT_SOURCE_DIR}/src/rvexception.cpp
    ${PROJECT_SOURCE_DIR}/src/rvexception.h
)

if (CMAKE_C_BYTE_ORDER STREQUAL LITTLE_ENDIAN)
    set(IS_LITTLE_ENDIAN 1)
else()
    set(IS_LITTLE_ENDIAN 0)
endif()

configure_file(src/cmake_config.h.in src/cmake_config.h)

add_library(libirve_static STATIC ${LIBIRVE_SOURCES})
add_library(libirve_shared SHARED ${LIBIRVE_SOURCES})
target_include_directories(libirve_static PRIVATE ${CMAKE_BINARY_DIR}/src/)
target_include_directories(libirve_shared PRIVATE ${CMAKE_BINARY_DIR}/src/)
set_property(TARGET libirve_static PROPERTY OUTPUT_NAME irve)
set_property(TARGET libirve_shared PROPERTY OUTPUT_NAME irve)

add_executable(irve ${IRVE_SOURCES})
target_link_libraries(irve PRIVATE libirve_shared)#TODO or should we make this static?
