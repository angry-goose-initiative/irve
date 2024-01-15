# CMakeLists.txt
# Copyright (C) 2023-2024 John Jekel
# See the LICENSE file at the root of the project for licensing info.
#
# CMake configuration file for the OGSBI (originally the IRVE SBI)
#
# Based on CMakeLists.txt from rv32esim
#

#Common options
cmake_minimum_required(VERSION 3.16.3)

include(${PROJECT_SOURCE_DIR}/macros.cmake)

set(OGSBI_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/asm_c_interface.h
    ${CMAKE_CURRENT_SOURCE_DIR}/common.h
    ${CMAKE_CURRENT_SOURCE_DIR}/dtb.S
    ${CMAKE_CURRENT_SOURCE_DIR}/exception_entry.s
    ${CMAKE_CURRENT_SOURCE_DIR}/gp_tp_preserve_restore.s
    ${CMAKE_CURRENT_SOURCE_DIR}/jump2linux.S
    ${CMAKE_CURRENT_SOURCE_DIR}/legacy.c
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/other_exceptions.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sbi.c
    ${CMAKE_CURRENT_SOURCE_DIR}/timers.c
    ${CMAKE_CURRENT_SOURCE_DIR}/virtual.S
)

macro(add_c_target NAME)
    add_target(${NAME} ${ARGN})

    #target_link_libraries(sbi__ogsbi__${NAME} rvswc)#TODO switch to this once we get it working
    workaround_add_needed_c_static_libs_for_target(sbi__ogsbi__${NAME})#The workaround
endmacro()

add_c_target(ogsbi ${OGSBI_SOURCES})
target_include_directories(sbi__ogsbi__ogsbi PRIVATE ${PROJECT_BINARY_DIR}/include)#To get the generated rvsw_asm.h
