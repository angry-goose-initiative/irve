# CMakeLists.txt
# Copyright (C) 2023-2024 John Jekel
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
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/ffi.rs
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/disassemble.rs
)

#TODO put the static library in a common area perhaps?
#TODO put the header in an include subdirectory in target perhaps?

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(LIBIRVE_DISASSEMBLE_OUTPUT ${PROJECT_BINARY_DIR}/rust/target/debug/libirve_disassemble.a ${PROJECT_BINARY_DIR}/rust/target/irve_disassemble.h)
    add_custom_command(
        OUTPUT ${LIBIRVE_DISASSEMBLE_OUTPUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS ${LIBIRVE_DISASSEMBLE_SOURCES}
        COMMAND cargo build --target-dir ${PROJECT_BINARY_DIR}/rust/target/
    )
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(LIBIRVE_DISASSEMBLE_OUTPUT ${PROJECT_BINARY_DIR}/rust/target/release/libirve_disassemble.a ${PROJECT_BINARY_DIR}/rust/target/irve_disassemble.h)
    add_custom_command(
        OUTPUT ${LIBIRVE_DISASSEMBLE_OUTPUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS ${LIBIRVE_DISASSEMBLE_SOURCES}
        COMMAND cargo build --target-dir ${PROJECT_BINARY_DIR}/rust/target/ --release 
    )
endif()

add_custom_target(libirve_disassemble DEPENDS ${LIBIRVE_DISASSEMBLE_OUTPUT})
set_property(
    TARGET libirve_disassemble
    APPEND PROPERTY ADDITIONAL_CLEAN_FILES "${PROJECT_BINARY_DIR}/rust/target/"
)
