# irve_rvsw_config.cmake
# Copyright (C) 2023 John Jekel and Nick Chan
# See the LICENSE file at the root of the project for licensing info.
#
# IRVE RVSW configuration file
#

set(CMAKE_ASM_COMPILER riscv32-unknown-elf-gcc)
set(CMAKE_C_COMPILER riscv32-unknown-elf-gcc)
set(CMAKE_CXX_COMPILER riscv32-unknown-elf-g++)
set(RVSW_OBJCOPY riscv32-unknown-elf-objcopy)

set(RVSW_DTB_PATH "${PROJECT_SOURCE_DIR}/../irve.dtb")

set(RVSW_COMMON_FLAGS "-fomit-frame-pointer -Wall -Wextra -Werror -nostartfiles -static -static-libgcc --specs=nosys.specs -mstrict-align")                          
set(RVSW_COMMON_FLAGS_DEBUG "-O0 -g3")
#set(RVSW_COMMON_FLAGS_RELEASE "-O3 -flto=auto -fuse-linker-plugin")#TODO re-enable when nommulinux issue is fixed
set(RVSW_COMMON_FLAGS_RELEASE "-O3")

set(RVSW_MMODE_LD_SCRIPT "${PROJECT_SOURCE_DIR}/../irve_mmode.ld")
set(RVSW_MTVEC_IS_HARDWIRED "VECTORED")
set(RVSW_MARCH "rv32ima_zicsr_zifencei")
set(RVSW_MABI "ilp32")

set(RVSW_EXIT_METHOD "CUSTOM_INSTRUCTION")
set(RVSW_CUSTOM_EXIT_INSTR ".insn r CUSTOM_0, 0, 0, zero, zero, zero")

set(RVSW_DATA_SEGMENT_ALREADY_LOADED "1")
set(RVSW_BSS_SEGMENT_ALREADY_ZEROED "0")

#Your own firmware must link against the SBI static library that is built
set(RVSW_BUILD_SBI "ogsbi")#Uncomment if you want to build an SBI (and specify the name of the one you want to build)

#TODO options for firmware

set(RVSW_SMODE_AND_KERNEL_ENTRY_ADDR 0xC0000000)

set(RVSW_MMODE_NEWLIB_SYSCALLS_STATICLIB_CMAKE_TARGET "irve_rvsw_newlib_syscalls")

set(RVSW_BUILD_SMODE_SW "1")#Set to 1 if you want to build supervisor mode software too
set(RVSW_SMODE_LD_SCRIPT "${PROJECT_SOURCE_DIR}/../irve_smode.ld")#Your linker script here (relative to the root of the rvsw checkout)
set(RVSW_STVEC_IS_HARDWIRED "VECTORED")#Uncomment if not hardwired, else "DIRECT" if direct, or "VECTORED" if vectored
set(RVSW_SMODE_DATA_SEGMENT_ALREADY_LOADED "1")#Set to 0 if you need the C startup code to load the data segment before calling anything
set(RVSW_SMODE_BSS_SEGMENT_ALREADY_ZEROED "0")#Set to 0 if you need the C startup code to zero the bss segment before calling anything
set(RVSW_SMODE_MARCH "rv32ima_zicsr_zifencei")#Your -march=[...] here (can be different from M-mode; ex if mmode code is emulating certain instructions)
set(RVSW_SMODE_MABI "ilp32")#Your -mabi=[...] here (can be different from M-mode)
