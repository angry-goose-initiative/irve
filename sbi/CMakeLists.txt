# CMakeLists.txt
# Copyright (C) 2023 John Jekel and Nick Chan
# See the LICENSE file at the root of the project for licensing info.
#
# CMake configuration file for the IRVE SBI
#
# Based on CMakeLists.txt from rv32esim
#

#Common options
cmake_minimum_required(VERSION 3.16.3)
project(irve_sbi VERSION 0.1.0 LANGUAGES C ASM)
#Needed for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#TODO recompile everything if irvesbi.ld changes

#https://stackoverflow.com/questions/41361631/optimize-in-cmake-by-default
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

#Compilers to use
set(CMAKE_C_COMPILER riscv32-unknown-elf-gcc)
set(CMAKE_ASM_COMPILER riscv32-unknown-elf-gcc)
set(OBJCOPY riscv32-unknown-elf-objcopy)

#Compile options
#TODO DON'T USE -ffreestanding for files that use Newlib or the special case hello_newlib.c
set(COMMON_FLAGS "-std=gnu18 -Wall -Wextra -Werror -march=rv32imazicsr -mabi=ilp32 -ffreestanding -nostartfiles -static -static-libgcc --specs=nosys.specs -mstrict-align -T ${CMAKE_CURRENT_SOURCE_DIR}/irvesbi.ld")
set(COMMON_FLAGS_DEBUG "-O0 -g3")
set(COMMON_FLAGS_RELEASE "-O3 -flto=auto -fuse-linker-plugin")
set(CMAKE_C_FLAGS "${COMMON_FLAGS}")
set(CMAKE_ASM_FLAGS "${COMMON_FLAGS}")
set(CMAKE_C_FLAGS_DEBUG "${COMMON_FLAGS_DEBUG}")
set(CMAKE_ASM_FLAGS_DEBUG "${COMMON_FLAGS_DEBUG}")
set(CMAKE_C_FLAGS_RELEASE "${COMMON_FLAGS_RELEASE}")
set(CMAKE_ASM_FLAGS_RELEASE "${COMMON_FLAGS_RELEASE}")
#set(OBJCOPY_FLAGS "-O verilog --verilog-data-width=4")#TODO this dosn't work for some reason

set(IRVE_SBI_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/exception_entry.s
    ${CMAKE_CURRENT_SOURCE_DIR}/jump2linux.s
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
)

add_executable(irve_sbi ${IRVE_SBI_SOURCES})

#Produce a Verilog hex file
add_custom_command(
    TARGET irve_sbi
    POST_BUILD
    COMMAND ${OBJCOPY} 
    #TODO use OBJCOPY_FLAGS instead of hardcoding
    #ARGS ${OBJCOPY_FLAGS}
    ARGS -O verilog --verilog-data-width=4 irve_sbi irve_sbi.hex
)
set_property(
    TARGET irve_sbi
    APPEND PROPERTY ADDITIONAL_CLEAN_FILES irve_sbi.hex
)

#Depends on cmake targets produced elsewhere
target_link_libraries(irve_sbi irvervc)#Get the IRVE bare-bones c runtime
target_link_libraries(irve_sbi irvenewlib)#To get Newlib syscalls for IRVE
target_link_libraries(irve_sbi c)#To get Newlib's C library
target_link_libraries(irve_sbi m)#To get Newlib's math library
target_link_libraries(irve_sbi gcc)#To get GCC's emulation routines
