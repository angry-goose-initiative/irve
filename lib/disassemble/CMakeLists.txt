# CMakeLists.txt
# Copyright (C) 2023 John Jekel and Nick Chan
# See the LICENSE file at the root of the project for licensing info.
#
# CMake configuration file for libirve_disassemble (integration with Cargo)
#
# Based on CMakeLists.txt from rv32esim
#

#Common options
cmake_minimum_required(VERSION 3.16.3)

set(LIBIRVE_DISASSEMBLE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Cargo.toml
    ${CMAKE_CURRENT_SOURCE_DIR}/build.rs
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/lib.rs
)

#TODO put the static library in a common area perhaps?
#TODO put the header in an include subdirectory in target perhaps?

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/target/debug/libirve_disassemble.a ${CMAKE_CURRENT_SOURCE_DIR}/target/irve_disassemble.h
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS ${LIBIRVE_DISASSEMBLE_SOURCES}
        COMMAND cargo build
    )
    add_custom_target(libirve_disassemble DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/target/debug/libirve_disassemble.a)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/target/release/libirve_disassemble.a ${CMAKE_CURRENT_SOURCE_DIR}/target/irve_disassemble.h
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS ${LIBIRVE_DISASSEMBLE_SOURCES}
        COMMAND cargo build --release
    )
    add_custom_target(libirve_disassemble DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/target/release/libirve_disassemble.a)
endif()

set_property(
    TARGET libirve_disassemble
    APPEND PROPERTY ADDITIONAL_CLEAN_FILES "${CMAKE_CURRENT_SOURCE_DIR}/target ${CMAKE_CURRENT_SOURCE_DIR}/Cargo.lock"
)
