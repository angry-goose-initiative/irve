# CMakeLists.txt
# Copyright (C) 2023 John Jekel and Nick Chan
# See the LICENSE file at the root of the project for licensing info.
#
# CMake configuration file for RVSW common c code
#
# Based on testfiles/CMakeLists.txt from irve
#

#Common options
cmake_minimum_required(VERSION 3.16.3)

#include(${PROJECT_SOURCE_DIR}/macros.cmake)

#macro(add_asm_target NAME)
#add_target(${NAME} ${ARGN})
#target_include_directories(src__single_file__asm__${NAME} PRIVATE ${PROJECT_BINARY_DIR}/src)#To get the generated rvsw_asm.h
#endmacro()

#Assembly test files from rv32esim
#add_asm_target(rv32esim rv32esim.S)

#TODO others

set(RVSW_C_RUNTIME_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/cinit.S
    ${CMAKE_CURRENT_SOURCE_DIR}/vector_table.S
    #TODO others
)

add_library(rvsw_c_runtime STATIC ${RVSW_C_RUNTIME_SOURCES})
target_include_directories(rvsw_c_runtime PRIVATE ${PROJECT_BINARY_DIR}/include)#To get the generated rvsw_asm.h

add_library(rvswc INTERFACE)#Represents several static libraries
target_link_libraries(rvswc INTERFACE rvsw_c_runtime)
target_link_libraries(rvswc INTERFACE gcc)#To get GCC's emulation routines
target_link_libraries(rvswc INTERFACE ${RVSW_MMODE_NEWLIB_SYSCALLS_STATICLIB_CMAKE_TARGET})#To get the user's implementation of Newlib syscalls
target_link_libraries(rvswc INTERFACE c)#To get Newlib's C library
target_link_libraries(rvswc INTERFACE m)#To get Newlib's math library

#[[

##Static library target for RISC-V C runtime setup and other abstractions over
#other IRVE features available to guest RISC-V programs
#RVC means "RISC-V C", so libirvervc is the "RISC-V C" guest runtime library for IRVE
#NOTE: NOT needed for assembly programs
set(LIBIRVERVC_SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/c/vector_table.s
    ${CMAKE_CURRENT_SOURCE_DIR}/src/c/crt0.s
    ${CMAKE_CURRENT_SOURCE_DIR}/src/c/irve.h
)

#Static library target for Newlib initialization, syscalls, etc. using libirvervc
#Depends on libirvervc
#Optional if you don't want to use Newlib
set(LIBIRVENEWLIB_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/c/irve_newlib.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/c/irve.h
)

 #NOTE: Using OBJECT libraries instead of STATIC libraries because of linker problems with static libraries and weak symbols
#Basically implementing Newlib syscalls overrides some weak symbols, but this dosn't work with static libraries for some reason
#(without --whole-archive, that is, which is a pain in CMake). So just use OBJECT libraries instead.
add_library(irvervc OBJECT ${LIBIRVERVC_SOURCES})
add_library(irvenewlib OBJECT ${LIBIRVENEWLIB_SOURCES})
target_include_directories(irvervc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/c)
target_include_directories(irvenewlib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/c)
target_link_libraries(irvenewlib irvervc)

#Static libraries are provided as an option though
add_library(irvervc_static STATIC $<TARGET_OBJECTS:irvervc>)
set_property(TARGET irvervc_static PROPERTY OUTPUT_NAME irvervc)
add_library(irvenewlib_static STATIC $<TARGET_OBJECTS:irvenewlib>)
set_property(TARGET irvenewlib_static PROPERTY OUTPUT_NAME irvenewlib)
target_link_libraries(irvenewlib_static irvervc_static)add_subdirectory(jzjcoresoftware)

#]]
