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

set(RVSW_C_RUNTIME_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/cinit.S
    ${CMAKE_CURRENT_SOURCE_DIR}/vector_table.S
    #TODO others
)

add_library(rvsw_c_runtime STATIC ${RVSW_C_RUNTIME_SOURCES})
target_include_directories(rvsw_c_runtime PRIVATE ${PROJECT_BINARY_DIR}/include)#To get the generated rvsw_asm.h

#As a workaround, use the workaround_add_needed_c_static_libs_for_target macro for the time being
#[[add_library(rvswc INTERFACE)#Represents several static libraries#FIXME this does not work
target_link_libraries(rvswc INTERFACE rvsw_c_runtime)
target_link_libraries(rvswc INTERFACE gcc)#To get GCC's emulation routines
target_link_libraries(rvswc INTERFACE ${RVSW_MMODE_NEWLIB_SYSCALLS_STATICLIB_CMAKE_TARGET})#To get the user's implementation of Newlib syscalls
target_link_libraries(rvswc INTERFACE c)#To get Newlib's C library
target_link_libraries(rvswc INTERFACE m)#To get Newlib's math library
#]]
