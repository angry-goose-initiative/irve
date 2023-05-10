# CMakeLists.txt
# Copyright (C) 2023 John Jekel and Nick Chan
# See the LICENSE file at the root of the project for licensing info.
#
# CMake configuration file for irve
#
# Based on CMakeLists.txt from rv32esim
#

#Common options
cmake_minimum_required(VERSION 3.16.3)
project(irve VERSION 0.2.0)
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

set(CMAKE_CXX_FLAGS "-Wall -std=c++20")
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
endif()

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
    ${PROJECT_SOURCE_DIR}/src/loader.cpp
    ${PROJECT_SOURCE_DIR}/src/loader.h
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

#Attempt to avoid needing to double compile everything:
#Foiled by the fact that we don't need PIC for static libraries, but we do for shared libraries
#https://stackoverflow.com/questions/50600708/combining-cmake-object-libraries-with-shared-libraries
#Otherwise we could have reused libirve_object for both
#We could make libirve_object position independent, but that would hurt libirve_static's performance unnecessarily
add_library(libirve_object OBJECT ${LIBIRVE_SOURCES})
target_include_directories(libirve_object PRIVATE ${CMAKE_BINARY_DIR}/src/)

add_library(libirve_static STATIC $<TARGET_OBJECTS:libirve_object>)
set_property(TARGET libirve_static PROPERTY OUTPUT_NAME irve)

#add_library(libirve_shared SHARED $<TARGET_OBJECTS:libirve_object>)#Foiled!
add_library(libirve_shared SHARED ${LIBIRVE_SOURCES})#The old way
target_include_directories(libirve_shared PRIVATE ${CMAKE_BINARY_DIR}/src/)#Also part of the old way
set_property(TARGET libirve_shared PROPERTY OUTPUT_NAME irve)

add_executable(irve ${IRVE_SOURCES})
target_include_directories(irve PRIVATE ${CMAKE_BINARY_DIR}/src/)#TODO we should really embed the version info into the library instead of this...
target_link_libraries(irve PRIVATE libirve_static)#TODO or should we make this shared?
