# CMakeLists.txt
# Copyright (C) 2023 John Jekel and Nick Chan
# See the LICENSE file at the root of the project for licensing info.
#
# CMake configuration file for libirve
#
# Based on CMakeLists.txt from rv32esim
#

#Common options
cmake_minimum_required(VERSION 3.16.3)

if (IRVE_RUST)
    add_subdirectory(disassemble)
endif()

#Gets variables from parent directory CMakeLists.txt files
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake_config.h.in ${CMAKE_CURRENT_BINARY_DIR}/cmake_config.h @ONLY)

set(LIBIRVE_SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/cmake_config.h
    ${CMAKE_CURRENT_SOURCE_DIR}/common.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/common.h
    ${CMAKE_CURRENT_SOURCE_DIR}/cpu_state.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpu_state.h
    ${CMAKE_CURRENT_SOURCE_DIR}/CSR.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CSR.h
    ${CMAKE_CURRENT_SOURCE_DIR}/decode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/decode.h
    ${CMAKE_CURRENT_SOURCE_DIR}/emulator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/emulator.h
    ${CMAKE_CURRENT_SOURCE_DIR}/execute.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/execute.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gdbserver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gdbserver.h
    ${CMAKE_CURRENT_SOURCE_DIR}/irve_public_api.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/logging.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/logging.h
    ${CMAKE_CURRENT_SOURCE_DIR}/memory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/memory.h
    ${CMAKE_CURRENT_SOURCE_DIR}/rvexception.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/rvexception.h
    ${CMAKE_SOURCE_DIR}/include/irve_public_api.h
)

#Attempt to avoid needing to double compile everything:
#Foiled by the fact that we don't need PIC for static libraries, but we do for shared libraries
#https://stackoverflow.com/questions/50600708/combining-cmake-object-libraries-with-shared-libraries
#Otherwise we could have reused libirve_object for both
#We could make libirve_object position independent, but that would hurt libirve_static's performance unnecessarily
add_library(libirve_object OBJECT ${LIBIRVE_SOURCES})
target_include_directories(libirve_object PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/)#To get the generated cmake_config.h
target_include_directories(libirve_object PRIVATE ${CMAKE_SOURCE_DIR}/include/)#Public API header(s)

#CHANGES: Actually, we're just biting the bullet and using -fPIC
#When irvers is linking against the static library, it still needs it to be fPIC anways...
set_property(TARGET libirve_object PROPERTY POSITION_INDEPENDENT_CODE ON)

add_library(libirve_static STATIC $<TARGET_OBJECTS:libirve_object>)
set_property(TARGET libirve_static PROPERTY OUTPUT_NAME irve)

add_library(libirve_shared SHARED $<TARGET_OBJECTS:libirve_object>)
#The old way
#add_library(libirve_shared SHARED ${LIBIRVE_SOURCES})
#target_include_directories(libirve_shared PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/)#To get the generated cmake_config.h
#target_include_directories(libirve_shared PRIVATE ${CMAKE_SOURCE_DIR}/include)#Public API header(s)
#End of the old way
set_property(TARGET libirve_shared PROPERTY OUTPUT_NAME irve)

if (IRVE_RUST)
    #TODO put the static library in a common area perhaps?
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_link_libraries(libirve_object PRIVATE ${PROJECT_BINARY_DIR}/rust/target/debug/libirve_disassemble.a)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_link_libraries(libirve_object PRIVATE ${PROJECT_BINARY_DIR}/rust/target/release/libirve_disassemble.a)
    endif()
    #TODO put the header in an include subdirectory in target perhaps?
    target_include_directories(libirve_object PRIVATE ${PROJECT_BINARY_DIR}/rust/target)#To get access to the Bindgen generated header
    add_dependencies(libirve_object libirve_disassemble)
endif()
