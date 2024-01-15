# CMakeLists.txt
# Copyright (C) 2023-2024 John Jekel
# See the LICENSE file at the root of the project for licensing info.
#
# CMake configuration file for jzjcoresoftware-originating tests
#
# Based on testfiles/CMakeLists.txt from irve
#

#Common options
cmake_minimum_required(VERSION 3.16.3)

include(${PROJECT_SOURCE_DIR}/macros.cmake)

macro(add_asm_target NAME)
    add_target(${NAME} ${ARGN})
    target_include_directories(src__single_file__asm__jzjcoresoftware__${NAME} PRIVATE ${PROJECT_BINARY_DIR}/include)#To get the generated rvsw_asm.h
endmacro()

#Assembly test files from jzjcoresoftware
macro(add_jzjcoresoftware_asm_target NAME)
    add_asm_target(${NAME} ${CMAKE_CURRENT_SOURCE_DIR}/${NAME}.S)
endmacro()
add_jzjcoresoftware_asm_target(adding2)
add_jzjcoresoftware_asm_target(auipctest)
add_jzjcoresoftware_asm_target(bneandsubtest) 
add_jzjcoresoftware_asm_target(callrettest)
add_jzjcoresoftware_asm_target(fenceecalltest)
add_jzjcoresoftware_asm_target(fibbonaccijal)
add_jzjcoresoftware_asm_target(fibbonaccijalr)
add_jzjcoresoftware_asm_target(lbutest)
add_jzjcoresoftware_asm_target(lhtest)
add_jzjcoresoftware_asm_target(lhutest)   
add_jzjcoresoftware_asm_target(loadbytetest)
add_jzjcoresoftware_asm_target(luitest)
add_jzjcoresoftware_asm_target(memoryreadtest)
add_jzjcoresoftware_asm_target(memorywritetest)
add_jzjcoresoftware_asm_target(nop)
add_jzjcoresoftware_asm_target(sbtest)
add_jzjcoresoftware_asm_target(sbtest2)   
add_jzjcoresoftware_asm_target(shtest)  
add_jzjcoresoftware_asm_target(sllisrliblttest)
add_jzjcoresoftware_asm_target(sllsrlblttest)
add_jzjcoresoftware_asm_target(testingfunctions)
add_jzjcoresoftware_asm_target(uncondjumptest)
add_jzjcoresoftware_asm_target(uncondjumptest2)
add_jzjcoresoftware_asm_target(xoritoggle)

#TODO others
