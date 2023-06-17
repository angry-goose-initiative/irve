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
    target_link_libraries(src__single_file__c__${NAME} rvswc)
endmacro()

#TODO actual targets
add_c_target(hello_world ${CMAKE_CURRENT_SOURCE_DIR}/hello_world.c)
