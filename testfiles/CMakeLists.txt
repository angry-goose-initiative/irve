# CMakeLists.txt
# Copyright (C) 2023 John Jekel and Nick Chan
# See the LICENSE file at the root of the project for licensing info.
#
# CMake configuration file for irve testfiles
#
# Based on CMakeLists.txt from rv32esim
#

#Common options
cmake_minimum_required(VERSION 3.21)
project(irve_tesfiles VERSION 0.1 LANGUAGES C ASM)
#Needed for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#https://stackoverflow.com/questions/41361631/optimize-in-cmake-by-default
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

#Compilers to use
#TODO need to probably add these to the path in the future
set(CMAKE_C_COMPILER /opt/riscv/bin/riscv32-unknown-elf-gcc)
set(CMAKE_ASM_COMPILER /opt/riscv/bin/riscv32-unknown-elf-gcc)
set(OBJCOPY /opt/riscv/bin/riscv32-unknown-elf-objcopy)

#Compile options
set(COMMON_FLAGS "-Wall -Wextra -march=rv32ima -mabi=ilp32 -nostartfiles -static -static-libgcc --specs=nosys.specs -lc -lgcc -mstrict-align -T ${CMAKE_CURRENT_SOURCE_DIR}/src/irve.ld")
set(CMAKE_C_FLAGS "${COMMON_FLAGS}")
set(CMAKE_ASM_FLAGS "${COMMON_FLAGS}")
#set(OBJCOPY_FLAGS "-O verilog --verilog-data-width=4")#TODO this dosn't work for some reason

#Static library targets for C runtime setup and other IRVE features
set(LIBIRVE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/c/crt0.s ${CMAKE_CURRENT_SOURCE_DIR}/src/c/irve_newlib.c)
set(LIBIRVE_NO_NEWLIB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/c/crt0_no_newlib.s)
add_library(irve STATIC ${LIBIRVE_SOURCES})
add_library(irve_no_newlib STATIC ${LIBIRVE_NO_NEWLIB_SOURCES})

#Macro for outputing Verilog hex files
macro(add_hex_target BINARY_NAME)
    add_custom_command(
        TARGET ${BINARY_NAME} 
        POST_BUILD
        COMMAND ${OBJCOPY} 
        #TODO use OBJCOPY_FLAGS instead of hardcoding
        #ARGS ${OBJCOPY_FLAGS}
        ARGS -O verilog --verilog-data-width=4 ${BINARY_NAME} ${BINARY_NAME}.hex
    )
    set_property(
        TARGET ${BINARY_NAME}
        APPEND PROPERTY ADDITIONAL_CLEAN_FILES ${BINARY_NAME}.hex
    )
endmacro()

###############################################################################
# Test executables
###############################################################################

############
# Assembly #
############

#IRVE assembly test files
#TODO

#Assembly test files from rv32esim
add_executable(rv32esim src/assembly/rv32esim.s)
add_hex_target(rv32esim)

#Assembly test files from jzjcoresoftware
macro(add_jzjcoresoftware_asm_target NAME)
    add_executable(${NAME} src/assembly/jzjcoresoftware/${NAME}.mem.s)
    add_hex_target(${NAME})
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

############
#     C    #
############

#IRVE C test files

#Without newlib
add_executable(hello_world src/c/without_c_stdlib/hello_world.c)
add_hex_target(hello_world)
target_link_libraries(hello_world irve_no_newlib)
add_executable(rv32im_sanity src/c/without_c_stdlib/rv32im_sanity.c)
add_hex_target(rv32im_sanity)
target_link_libraries(rv32im_sanity irve_no_newlib)
add_executable(software_floating_point_fun src/c/without_c_stdlib/software_floating_point_fun.c)
add_hex_target(software_floating_point_fun)
target_link_libraries(software_floating_point_fun irve_no_newlib)

#Special case (implement newlib syscalls themselves)
add_executable(hello_newlib src/c/with_c_stdlib/hello_newlib.c)
add_hex_target(hello_newlib)
target_link_libraries(hello_newlib irve_no_newlib)

#With newlib
#TODO fix warnings that Newlib cant find the necessary syscalls?
add_executable(stress_test src/c/with_c_stdlib/stress_test.c)
add_hex_target(stress_test)
target_link_libraries(stress_test irve)
#TODO

#C test files from rv32esim (All without newlib)
add_executable(rv32esim_c src/c/without_c_stdlib/rv32esim_c.c)
add_hex_target(rv32esim_c)
target_link_libraries(rv32esim_c irve_no_newlib)

#C test files from jzjcoresoftware (All without newlib)
#TODO
