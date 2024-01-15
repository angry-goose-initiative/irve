# CMakeLists.txt
# Copyright (C) 2023-2024 John Jekel
# See the LICENSE file at the root of the project for licensing info.
#
# CMake configuration file for single file c tests
#
# Based on CMakeLists.txt from irve
#

#Common options
cmake_minimum_required(VERSION 3.16.3)
#
include(${PROJECT_SOURCE_DIR}/macros.cmake)

macro(add_cxx_target NAME)
    add_target(${NAME} ${ARGN})

    target_link_libraries(src__single_file__cxx__cppreference__${NAME} rvsw_cxx_runtime)

    #target_link_libraries(src__single_file__cxx__${NAME} rvswc)#TODO switch to this once we get it working
    workaround_add_needed_c_static_libs_for_target(src__single_file__cxx__cppreference__${NAME})#The workaround
    
endmacro()

macro(add_smode_cxx_target NAME)
    add_smode_target(${NAME} ${ARGN})

    target_link_libraries(src__single_file__cxx__cppreference__${NAME}_smode rvsw_cxx_runtime_smode)

    #target_link_libraries(src__single_file__cxx__${NAME}_smode rvswc)#TODO switch to this once we get it working
    smode_workaround_add_needed_c_static_libs_for_target(src__single_file__cxx__cppreference__${NAME}_smode)#The workaround
endmacro()

macro(add_double_target NAME)
    add_cxx_target(${NAME} ${CMAKE_CURRENT_SOURCE_DIR}/${NAME}.cpp)
    add_smode_cxx_target(${NAME} ${CMAKE_CURRENT_SOURCE_DIR}/${NAME}.cpp)
endmacro()

#Targets
add_double_target(dequeue)
add_double_target(integer_sequence)
add_double_target(map)
add_double_target(string)
add_double_target(tuple)
