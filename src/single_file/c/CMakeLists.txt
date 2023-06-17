# CMakeLists.txt
# Copyright (C) 2023 John Jekel and Nick Chan
# See the LICENSE file at the root of the project for licensing info.
#
# CMake configuration file for single file c tests
#
# Based on CMakeLists.txt from irve
#

#Common options
cmake_minimum_required(VERSION 3.16.3)

include(${PROJECT_SOURCE_DIR}/macros.cmake)

macro(add_c_target NAME)
    add_target(${NAME} ${ARGN})

    #target_link_libraries(src__single_file__c__${NAME} rvswc)#TODO switch to this once we get it working

    #The workaround
    target_link_libraries(src__single_file__c__${NAME} rvsw_c_runtime)
    target_link_libraries(src__single_file__c__${NAME} gcc)#To get GCC's emulation routines
    target_link_libraries(src__single_file__c__${NAME} ${RVSW_MMODE_NEWLIB_SYSCALLS_STATICLIB_CMAKE_TARGET})#To get the user's implementation of Newlib syscalls
    target_link_libraries(src__single_file__c__${NAME} c)#To get Newlib's C library
    target_link_libraries(src__single_file__c__${NAME} m)#To get Newlib's math library
endmacro()

#TODO actual targets
add_c_target(hello_world ${CMAKE_CURRENT_SOURCE_DIR}/hello_world.c)
