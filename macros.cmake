# macros.cmake
# Copyright (C) 2023 John Jekel and Nick Chan
# See the LICENSE file at the root of the project for licensing info.
#
# Collection of common CMake macros
#

###############################################################################
# Output Formats
###############################################################################

#Macro for outputing 8-bit Verilog hex files
macro(add_vhex8_target TARGET_NAME ELF_NAME)
    add_custom_command(
        TARGET ${TARGET_NAME}
        POST_BUILD
        COMMAND ${RVSW_OBJCOPY} 
        #TODO use OBJCOPY_FLAGS instead of hardcoding
        #ARGS ${OBJCOPY_FLAGS}
        ARGS -O verilog ${ELF_NAME}.elf ${ELF_NAME}.vhex8
    )
    set_property(
        TARGET ${TARGET_NAME}
        APPEND PROPERTY ADDITIONAL_CLEAN_FILES ${ELF_NAME}.vhex8
    )
endmacro()

#Macro for outputing 16-bit Verilog hex files
macro(add_vhex16_target TARGET_NAME ELF_NAME)
    add_custom_command(
        TARGET ${TARGET_NAME}
        POST_BUILD
        COMMAND ${RVSW_OBJCOPY} 
        #TODO use OBJCOPY_FLAGS instead of hardcoding
        #ARGS ${OBJCOPY_FLAGS}
        ARGS -O verilog --verilog-data-width=2 ${ELF_NAME}.elf ${ELF_NAME}.vhex16
    )
    set_property(
        TARGET ${TARGET_NAME}
        APPEND PROPERTY ADDITIONAL_CLEAN_FILES ${ELF_NAME}.vhex16
    )
endmacro()

#Macro for outputing 32-bit Verilog hex files
macro(add_vhex32_target TARGET_NAME ELF_NAME)
    add_custom_command(
        TARGET ${TARGET_NAME}
        POST_BUILD
        COMMAND ${RVSW_OBJCOPY} 
        #TODO use OBJCOPY_FLAGS instead of hardcoding
        #ARGS ${OBJCOPY_FLAGS}
        ARGS -O verilog --verilog-data-width=4 ${ELF_NAME}.elf ${ELF_NAME}.vhex32
    )
    set_property(
        TARGET ${TARGET_NAME}
        APPEND PROPERTY ADDITIONAL_CLEAN_FILES ${ELF_NAME}.vhex32
    )
endmacro()

#Macro for outputing raw binary files
macro(add_bin_target TARGET_NAME ELF_NAME)
    add_custom_command(
        TARGET ${TARGET_NAME}
        POST_BUILD
        COMMAND ${RVSW_OBJCOPY} 
        #TODO use OBJCOPY_FLAGS instead of hardcoding
        #ARGS ${OBJCOPY_FLAGS}
        ARGS -O binary ${ELF_NAME}.elf ${ELF_NAME}.bin
    )
    set_property(
        TARGET ${TARGET_NAME}
        APPEND PROPERTY ADDITIONAL_CLEAN_FILES ${ELF_NAME}.bin
    )
endmacro()

#Macro for outputing Intel hexidecimal files
macro(add_ihex_target TARGET_NAME ELF_NAME)
    add_custom_command(
        TARGET ${TARGET_NAME}
        POST_BUILD
        COMMAND ${RVSW_OBJCOPY} 
        #TODO use OBJCOPY_FLAGS instead of hardcoding
        #ARGS ${OBJCOPY_FLAGS}
        ARGS -O ihex ${ELF_NAME}.elf ${ELF_NAME}.ihex
    )
    set_property(
        TARGET ${TARGET_NAME}
        APPEND PROPERTY ADDITIONAL_CLEAN_FILES ${ELF_NAME}.ihex
    )
endmacro()

#Macro for outputing srec hex files
macro(add_srec_target TARGET_NAME ELF_NAME)
    add_custom_command(
        TARGET ${TARGET_NAME}
        POST_BUILD
        COMMAND ${RVSW_OBJCOPY} 
        #TODO use OBJCOPY_FLAGS instead of hardcoding
        #ARGS ${OBJCOPY_FLAGS}
        ARGS -O srec ${ELF_NAME}.elf ${ELF_NAME}.srec
    )
    set_property(
        TARGET ${TARGET_NAME}
        APPEND PROPERTY ADDITIONAL_CLEAN_FILES ${ELF_NAME}.srec
    )
endmacro()

###############################################################################
# Target Creation
###############################################################################

#TODO create macro to get full name

macro(add_target NAME)
    file(RELATIVE_PATH FULLY_QUALIFIED_TARGET_NAME ${PROJECT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
    string(REPLACE "/" "__" FULLY_QUALIFIED_TARGET_NAME ${FULLY_QUALIFIED_TARGET_NAME})
    set(FULLY_QUALIFIED_TARGET_NAME "${FULLY_QUALIFIED_TARGET_NAME}__${NAME}")

    add_executable(${FULLY_QUALIFIED_TARGET_NAME} ${ARGN})

    #We output several different file formats for each target
    set_target_properties(${FULLY_QUALIFIED_TARGET_NAME} PROPERTIES OUTPUT_NAME ${NAME} SUFFIX ".elf")
    add_vhex8_target(${FULLY_QUALIFIED_TARGET_NAME} ${NAME})
    add_vhex16_target(${FULLY_QUALIFIED_TARGET_NAME} ${NAME})
    add_vhex32_target(${FULLY_QUALIFIED_TARGET_NAME} ${NAME})
    add_bin_target(${FULLY_QUALIFIED_TARGET_NAME} ${NAME})
    add_ihex_target(${FULLY_QUALIFIED_TARGET_NAME} ${NAME})
    add_srec_target(${FULLY_QUALIFIED_TARGET_NAME} ${NAME})
endmacro()

#TODO add more in the future
