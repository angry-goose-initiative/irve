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

include(${PROJECT_SOURCE_DIR}/macros.cmake)

macro(add_cxx_target NAME)
    add_target(${NAME} ${ARGN})

    target_link_libraries(src__single_file__cxx__${NAME} rvsw_cxx_runtime)

    #target_link_libraries(src__single_file__cxx__${NAME} rvswc)#TODO switch to this once we get it working
    workaround_add_needed_c_static_libs_for_target(src__single_file__cxx__${NAME})#The workaround
    
endmacro()

macro(add_smode_cxx_target NAME)
    add_smode_target(${NAME} ${ARGN})

    target_link_libraries(src__single_file__cxx__${NAME}_smode rvsw_cxx_runtime_smode)

    #target_link_libraries(src__single_file__cxx__${NAME}_smode rvswc)#TODO switch to this once we get it working
    smode_workaround_add_needed_c_static_libs_for_target(src__single_file__cxx__${NAME}_smode)#The workaround
endmacro()

#M-Mode Targets
add_cxx_target(cxx_experiments ${CMAKE_CURRENT_SOURCE_DIR}/cxx_experiments.cpp)
add_cxx_target(hello_cxx ${CMAKE_CURRENT_SOURCE_DIR}/hello_cxx.cpp)
add_cxx_target(irve_interrupt_bringup ${CMAKE_CURRENT_SOURCE_DIR}/irve_interrupt_bringup.cpp)

#S-Mode Targets
add_smode_cxx_target(cxx_experiments ${CMAKE_CURRENT_SOURCE_DIR}/cxx_experiments.cpp)
add_smode_cxx_target(hello_cxx ${CMAKE_CURRENT_SOURCE_DIR}/hello_cxx.cpp)
add_smode_cxx_target(irve_vm_bringup ${CMAKE_CURRENT_SOURCE_DIR}/irve_vm_bringup.cpp)
add_smode_cxx_target(morevm ${CMAKE_CURRENT_SOURCE_DIR}/morevm.cpp)

add_subdirectory(cppreference)
