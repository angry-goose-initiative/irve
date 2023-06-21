# CMakeLists.txt
# Copyright (C) 2023 John Jekel and Nick Chan
# See the LICENSE file at the root of the project for licensing info.
#
# CMake configuration file for irve executable
#
# Based on CMakeLists.txt from rv32esim
#

#Common options
cmake_minimum_required(VERSION 3.16.3)

if (IRVE_RUST)
    add_subdirectory(irvers)
endif()

set(IRVE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_SOURCE_DIR}/include/irve_public_api.h
)

set(IRVEGDB_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/maingdb.cpp
    ${CMAKE_SOURCE_DIR}/include/irve_public_api.h
)

add_executable(irve ${IRVE_SOURCES})
target_include_directories(irve PRIVATE ${CMAKE_SOURCE_DIR}/include)#Just using the public API
target_link_libraries(irve PRIVATE libirve_object)#TODO or should we make this static or shared instead?

add_executable(irvegdb ${IRVEGDB_SOURCES})
target_include_directories(irvegdb PRIVATE ${CMAKE_SOURCE_DIR}/include)#Just using the public API
target_link_libraries(irvegdb PRIVATE libirve_object)#TODO or should we make this static or shared instead?
