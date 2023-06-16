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
    add_executable(${NAME} ${ARGN})

    #We output several different file formats for each target
    set_target_properties(${NAME} PROPERTIES SUFFIX ".elf")
    add_vhex8_target(${NAME})
    add_vhex16_target(${NAME})
    add_vhex32_target(${NAME})
    add_bin_target(${NAME})
    add_ihex_target(${NAME})
    add_srec_target(${NAME})
endmacro()

############
# Assembly #
############
macro(add_asm_target NAME)
    add_target(${NAME} ${ARGN})
    target_include_directories(${NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/..)#To get the generated rvsw_asm.h
endmacro()

#Assembly test files from rv32esim
add_asm_target(rv32esim asm/rv32esim.S)

#TODO others
