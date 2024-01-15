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

macro(add_c_target NAME)
    add_target(${NAME} ${ARGN})

    #target_link_libraries(src__single_file__c__${NAME} rvswc)#TODO switch to this once we get it working
    workaround_add_needed_c_static_libs_for_target(src__single_file__c__${NAME})#The workaround
endmacro()

macro(add_smode_c_target NAME)
    add_smode_target(${NAME} ${ARGN})

    #target_link_libraries(src__single_file__c__${NAME}_smode rvswc)#TODO switch to this once we get it working
    smode_workaround_add_needed_c_static_libs_for_target(src__single_file__c__${NAME}_smode)#The workaround
endmacro()

#M-Mode Targets
add_c_target(hello_exceptions ${CMAKE_CURRENT_SOURCE_DIR}/hello_exceptions.c)#TODO only enable this if we have Zicsr
add_c_target(hello_world ${CMAKE_CURRENT_SOURCE_DIR}/hello_world.c)
add_c_target(irve_csr_bringup ${CMAKE_CURRENT_SOURCE_DIR}/irve_csr_bringup.c)#TODO only enable this if we have Zicsr
add_c_target(irve_debugging_puts_weirdness ${CMAKE_CURRENT_SOURCE_DIR}/irve_debugging_puts_weirdness.c)
add_c_target(irve_exception_bringup ${CMAKE_CURRENT_SOURCE_DIR}/irve_exception_bringup.c)#TODO only enable this if we have Zicsr
add_c_target(irve_stress_test ${CMAKE_CURRENT_SOURCE_DIR}/irve_stress_test.c)
add_c_target(nommulinux ${CMAKE_CURRENT_SOURCE_DIR}/nommulinux.c)
add_c_target(nouveau_stress_test ${CMAKE_CURRENT_SOURCE_DIR}/nouveau_stress_test.c)
add_c_target(poll_timer_interrupt_mmode ${CMAKE_CURRENT_SOURCE_DIR}/poll_timer_interrupt_mmode.c)
add_c_target(rv32esim ${CMAKE_CURRENT_SOURCE_DIR}/rv32esim.c)
add_c_target(rv32im_sanity ${CMAKE_CURRENT_SOURCE_DIR}/rv32im_sanity.c)
add_c_target(software_floating_point_fun ${CMAKE_CURRENT_SOURCE_DIR}/software_floating_point_fun.c)

#S-Mode Targets
add_smode_c_target(hello_world ${CMAKE_CURRENT_SOURCE_DIR}/hello_world.c)
add_smode_c_target(irve_debugging_puts_weirdness ${CMAKE_CURRENT_SOURCE_DIR}/irve_debugging_puts_weirdness.c)
add_smode_c_target(irve_stress_test ${CMAKE_CURRENT_SOURCE_DIR}/irve_stress_test.c)
add_smode_c_target(nouveau_stress_test ${CMAKE_CURRENT_SOURCE_DIR}/nouveau_stress_test.c)
add_smode_c_target(rv32esim ${CMAKE_CURRENT_SOURCE_DIR}/rv32esim.c)
add_smode_c_target(rv32im_sanity ${CMAKE_CURRENT_SOURCE_DIR}/rv32im_sanity.c)
add_smode_c_target(software_floating_point_fun ${CMAKE_CURRENT_SOURCE_DIR}/software_floating_point_fun.c)
#TODO others



#Additional special things
target_include_directories(src__single_file__c__nommulinux PRIVATE ${PROJECT_BINARY_DIR}/include)#To get the generated rvsw_asm.h
