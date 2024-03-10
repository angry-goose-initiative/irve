# CMakeLists.txt
# Copyright (C) 2023-2024 John Jekel
# See the LICENSE file at the root of the project for licensing info.
#
# CMake configuration file IRVE's usage of rvsw
#

cmake_minimum_required(VERSION 3.16.3)

###################################################################################################
# RVSW Setup
###################################################################################################

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/rvsw/.git")
    execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive "${CMAKE_CURRENT_SOURCE_DIR}/rvsw" WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()

set(RVSW_CONFIG_PATH "${CMAKE_CURRENT_SOURCE_DIR}/irve_rvsw_config.cmake")
add_subdirectory(rvsw)

###################################################################################################
# IRVE RVSW Newlib Syscalls
###################################################################################################

#Compilers to use
set(CMAKE_C_COMPILER riscv32-unknown-elf-gcc)
set(CMAKE_ASM_COMPILER riscv32-unknown-elf-gcc)
set(OBJCOPY riscv32-unknown-elf-objcopy)

#Compile options
#TODO DON'T USE -ffreestanding for files that use Newlib or the special case hello_newlib.c
set(COMMON_FLAGS "-std=gnu18 -fomit-frame-pointer -Wall -Wextra -Werror -march=rv32iazicsr -mabi=ilp32 -ffreestanding -nostartfiles -static -static-libgcc --specs=nosys.specs -mstrict-align")
set(COMMON_FLAGS_DEBUG "-O0 -g3")
set(COMMON_FLAGS_RELEASE "-O3 -flto=auto -fuse-linker-plugin")
set(CMAKE_C_FLAGS "${COMMON_FLAGS}")
set(CMAKE_ASM_FLAGS "${COMMON_FLAGS}")
set(CMAKE_C_FLAGS_DEBUG "${COMMON_FLAGS_DEBUG}")
set(CMAKE_ASM_FLAGS_DEBUG "${COMMON_FLAGS_DEBUG}")
set(CMAKE_C_FLAGS_RELEASE "${COMMON_FLAGS_RELEASE}")
set(CMAKE_ASM_FLAGS_RELEASE "${COMMON_FLAGS_RELEASE}")
set(IRVE_RVSW_NEWLIB_SYSCALLS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/irve_mmode_newlib.c
    ${CMAKE_CURRENT_SOURCE_DIR}/irve_mmode.h
)
add_library(irve_rvsw_newlib_syscalls OBJECT ${IRVE_RVSW_NEWLIB_SYSCALLS_SOURCES})
