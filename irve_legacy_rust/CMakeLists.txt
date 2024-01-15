# CMakeLists.txt
# Copyright (C) 2023-2024 John Jekel
# See the LICENSE file at the root of the project for licensing info.
#
# CMake configuration file for rust testfiles (integration with Cargo)
#
# Based on CMakeLists.txt from rv32esim
#

#Common options
cmake_minimum_required(VERSION 3.16.3)

set(RUST_TESTFILES_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Cargo.toml
    #TODO others
)

#if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    #set(RUST_TESTFILES_OUTPUT ${PROJECT_BINARY_DIR}/rust/target/debug/irvers)#TODO many files will be output
    #    add_custom_command(
        #OUTPUT ${IRVERS_OUTPUT}
        #    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        #DEPENDS ${IRVERS_SOURCES}
        #    COMMAND cargo build
        #)
#[[elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(IRVERS_OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/target/release/irvers)
    add_custom_command(
        OUTPUT ${IRVERS_OUTPUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS ${IRVERS_SOURCES}
        COMMAND cargo build --release
    )
endif()

#add_custom_target(rust_testfiles DEPENDS ${IRVERS_OUTPUT})
#add_dependencies(irvers libirve_static)
set_property(
    TARGET irvers
    APPEND PROPERTY ADDITIONAL_CLEAN_FILES "${CMAKE_CURRENT_SOURCE_DIR}/target ${CMAKE_CURRENT_SOURCE_DIR}/Cargo.lock"
)
]]
