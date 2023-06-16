# CMakeLists.txt
# Copyright (C) 2023 John Jekel and Nick Chan
# See the LICENSE file at the root of the project for licensing info.
#
# CMake configuration file for single_file
#
# Based on testfiles/CMakeLists.txt from irve
#

#Common options
cmake_minimum_required(VERSION 3.16.3)

###############################################################################
# Test executables
###############################################################################

#TODO add others as desired

#Macro for outputing 8-bit Verilog hex files
macro(add_vhex8_target BINARY_NAME)
    add_custom_command(
        TARGET ${BINARY_NAME}
        POST_BUILD
        COMMAND ${RVSW_OBJCOPY} 
        #TODO use OBJCOPY_FLAGS instead of hardcoding
        #ARGS ${OBJCOPY_FLAGS}
        ARGS -O verilog ${BINARY_NAME}.elf ${BINARY_NAME}.vhex8
    )
    set_property(
        TARGET ${BINARY_NAME}
        APPEND PROPERTY ADDITIONAL_CLEAN_FILES ${BINARY_NAME}.vhex8
    )
endmacro()

#Macro for outputing 16-bit Verilog hex files
macro(add_vhex16_target BINARY_NAME)
    add_custom_command(
        TARGET ${BINARY_NAME}
        POST_BUILD
        COMMAND ${RVSW_OBJCOPY} 
        #TODO use OBJCOPY_FLAGS instead of hardcoding
        #ARGS ${OBJCOPY_FLAGS}
        ARGS -O verilog --verilog-data-width=2 ${BINARY_NAME}.elf ${BINARY_NAME}.vhex16
    )
    set_property(
        TARGET ${BINARY_NAME}
        APPEND PROPERTY ADDITIONAL_CLEAN_FILES ${BINARY_NAME}.vhex16
    )
endmacro()

#Macro for outputing 32-bit Verilog hex files
macro(add_vhex32_target BINARY_NAME)
    add_custom_command(
        TARGET ${BINARY_NAME}
        POST_BUILD
        COMMAND ${RVSW_OBJCOPY} 
        #TODO use OBJCOPY_FLAGS instead of hardcoding
        #ARGS ${OBJCOPY_FLAGS}
        ARGS -O verilog --verilog-data-width=4 ${BINARY_NAME}.elf ${BINARY_NAME}.vhex32
    )
    set_property(
        TARGET ${BINARY_NAME}
        APPEND PROPERTY ADDITIONAL_CLEAN_FILES ${BINARY_NAME}.vhex32
    )
endmacro()

#Macro for outputing raw binary files
macro(add_bin_target BINARY_NAME)
    add_custom_command(
        TARGET ${BINARY_NAME}
        POST_BUILD
        COMMAND ${RVSW_OBJCOPY} 
        #TODO use OBJCOPY_FLAGS instead of hardcoding
        #ARGS ${OBJCOPY_FLAGS}
        ARGS -O binary ${BINARY_NAME}.elf ${BINARY_NAME}.bin
    )
    set_property(
        TARGET ${BINARY_NAME}
        APPEND PROPERTY ADDITIONAL_CLEAN_FILES ${BINARY_NAME}.bin
    )
endmacro()

#Macro for outputing Intel hexidecimal files
macro(add_ihex_target BINARY_NAME)
    add_custom_command(
        TARGET ${BINARY_NAME}
        POST_BUILD
        COMMAND ${RVSW_OBJCOPY} 
        #TODO use OBJCOPY_FLAGS instead of hardcoding
        #ARGS ${OBJCOPY_FLAGS}
        ARGS -O ihex ${BINARY_NAME}.elf ${BINARY_NAME}.ihex
    )
    set_property(
        TARGET ${BINARY_NAME}
        APPEND PROPERTY ADDITIONAL_CLEAN_FILES ${BINARY_NAME}.ihex
    )
endmacro()

#Macro for outputing srec hex files
macro(add_srec_target BINARY_NAME)
    add_custom_command(
        TARGET ${BINARY_NAME}
        POST_BUILD
        COMMAND ${RVSW_OBJCOPY} 
        #TODO use OBJCOPY_FLAGS instead of hardcoding
        #ARGS ${OBJCOPY_FLAGS}
        ARGS -O srec ${BINARY_NAME}.elf ${BINARY_NAME}.srec
    )
    set_property(
        TARGET ${BINARY_NAME}
        APPEND PROPERTY ADDITIONAL_CLEAN_FILES ${BINARY_NAME}.srec
    )
endmacro()

#Macro for adding targets
macro(add_target NAME)
    add_executable(single_file_${NAME} ${ARGN})

    #We output several different file formats for each target
    set_target_properties(single_file_${NAME} PROPERTIES SUFFIX ".elf")
    add_vhex8_target(single_file_${NAME})
    add_vhex16_target(single_file_${NAME})
    add_vhex32_target(single_file_${NAME})
    add_bin_target(single_file_${NAME})
    add_ihex_target(single_file_${NAME})
    add_srec_target(single_file_${NAME})
endmacro()

############
# Assembly #
############
macro(add_asm_target NAME)
    message(STATUS "Adding assembly target ${NAME}")
    add_target(asm_${NAME} ${ARGN})
    target_include_directories(single_file_asm_${NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/..)#To get the generated rvsw_asm.h
endmacro()

#Assembly test files from jzjcoresoftware
macro(add_jzjcoresoftware_asm_target NAME)
    add_asm_target(jzjcoresoftware_${NAME} ${CMAKE_CURRENT_SOURCE_DIR}/asm/jzjcoresoftware/${NAME}.S)
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

#Assembly test files from rv32esim
add_asm_target(rv32esim asm/rv32esim.S)

#TODO others
