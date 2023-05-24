# CMakeLists.txt
# Copyright (C) 2023 John Jekel and Nick Chan
# See the LICENSE file at the root of the project for licensing info.
#
# CMake configuration file for irvers (integration with Cargo)
#
# Based on CMakeLists.txt from rv32esim
#

#Common options
cmake_minimum_required(VERSION 3.16.3)

set(IRVERS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Cargo.toml
    ${CMAKE_CURRENT_SOURCE_DIR}/build.rs
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.rs
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/lib.rs
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/ffi.rs
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(IRVERS_OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/target/debug/irvers)
    add_custom_command(
        OUTPUT ${IRVERS_OUTPUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS ${IRVERS_SOURCES}
        COMMAND cargo build
    )
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(IRVERS_OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/target/release/irvers)
    add_custom_command(
        OUTPUT ${IRVERS_OUTPUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS ${IRVERS_SOURCES}
        COMMAND cargo build --release
    )
endif()

add_custom_target(irvers DEPENDS ${IRVERS_OUTPUT})
add_dependencies(irvers libirve_static)
set_property(
    TARGET irvers
    APPEND PROPERTY ADDITIONAL_CLEAN_FILES "${CMAKE_CURRENT_SOURCE_DIR}/target ${CMAKE_CURRENT_SOURCE_DIR}/Cargo.lock"
)
