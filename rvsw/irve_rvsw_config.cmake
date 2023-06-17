# irve_rvsw_config.cmake
# Copyright (C) 2023 John Jekel and Nick Chan
# See the LICENSE file at the root of the project for licensing info.
#
# IRVE RVSW configuration file
#

set(CMAKE_C_COMPILER riscv32-unknown-elf-gcc)
set(CMAKE_ASM_COMPILER riscv32-unknown-elf-gcc)
set(RVSW_OBJCOPY riscv32-unknown-elf-objcopy)

set(RVSW_COMMON_FLAGS "-fomit-frame-pointer -Wall -Wextra -Werror -ffreestanding -nostartfiles -static -static-libgcc --specs=nosys.specs -mstrict-align")                          
set(RVSW_COMMON_FLAGS_DEBUG "-O0 -g3")
set(RVSW_COMMON_FLAGS_RELEASE "-O3 -flto=auto -fuse-linker-plugin")

set(RVSW_MMODE_LD_SCRIPT "${PROJECT_SOURCE_DIR}/../irve_mmode.ld")
set(RVSW_MTVEC_IS_HARDWIRED "VECTORED")
set(RVSW_MARCH "rv32ima_zicsr")
set(RVSW_MABI "ilp32")

set(RVSW_EXIT_METHOD "CUSTOM_INSTRUCTION")
set(RVSW_CUSTOM_EXIT_INSTR ".insn r CUSTOM_0, 0, 0, zero, zero, zero")

set(RVSW_DATA_SEGMENT_ALREADY_LOADED "1")
set(RVSW_BSS_SEGMENT_ALREADY_ZEROED "0")

#Your own firmware must link against the SBI static library that is built
set(RVSW_BUILD_SBI "ogsbi")#Uncomment if you want to build an SBI (and specify the name of the one you want to build)

#TODO options for firmware

set(RVSW_MMODE_NEWLIB_SYSCALLS_STATICLIB_CMAKE_TARGET "irve_rvsw_newlib_syscalls")

set(RVSW_BUILD_SMODE_SW "1")#Uncomment if you want to build supervisor mode software too
#TODO other options
