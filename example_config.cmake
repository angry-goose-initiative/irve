# example_config.cmake
# Copyright (C) 2023 John Jekel and Nick Chan
# See the LICENSE file at the root of the project for licensing info.
#
# Example configuration file for rvsw
#

set(CMAKE_ASM_COMPILER riscv32-unknown-elf-gcc)
set(CMAKE_C_COMPILER riscv32-unknown-elf-gcc)  
set(CMAKE_CXX_COMPILER riscv32-unknown-elf-g++)
set(RVSW_OBJCOPY riscv32-unknown-elf-objcopy)

set(RVSW_DTB_PATH "/dev/null")#Your dtb path here (must be precompiled)

set(RVSW_COMMON_FLAGS "-fomit-frame-pointer -Wall -Wextra -Werror -ffreestanding -nostartfiles -static -static-libgcc --specs=nosys.specs -mstrict-align")
set(RVSW_MMODE_COMMON_FLAGS "")
set(RVSW_SMODE_COMMON_FLAGS "")
set(RVSW_COMMON_FLAGS_DEBUG "-O0 -g3")
set(RVSW_COMMON_FLAGS_RELEASE "-O3 -flto=auto -fuse-linker-plugin")

set(RVSW_MMODE_LD_SCRIPT "${PROJECT_SOURCE_DIR}/example_linker_script.ld")#Your linker script here (relative to the root of the rvsw checkout)

#Actually, the linker script should handle placement of the init and vector_table sections, so we don't need addresses: just vectored/non-vectored
#set(RVSW_ENTRY_ADDR "0x00000000")#Your entry address here
set(RVSW_MTVEC_IS_HARDWIRED "VECTORED")#Uncomment if not hardwired, else "DIRECT" if direct, or "VECTORED" if vectored
#set(RVSW_MTVEC_HARDWIRED_ADDR "0x00000004")#MTVEC hardwired address if applicable

set(RVSW_DATA_SEGMENT_ALREADY_LOADED "1")#Set to 0 if you need the C startup code to load the data segment before calling anything
set(RVSW_BSS_SEGMENT_ALREADY_ZEROED "0")#Set to 0 if you need the C startup code to zero the bss segment before calling anything

#Required linker script sections:
#.reset <- The cpu should begin execution at the first instruction of this section
#.vector_table <- Needed only if vector table is hardwired
#.text
#.bss and .sbss MUST BOTH BE SURROUNDED BY ___rvsw_bss_start___ and ___rvsw_bss_end___ TO AVOID ISSUES WITH VARIABLES THAT ARE ZERO-INITIALIZED
#TODO others

#Required linker script defined symbols:
#TODO

#The linker script should also contain ENTRY(___rvsw_reset___) in the file

set(RVSW_SMODE_AND_KERNEL_ENTRY_ADDR 0xC0000000)#Your entry address here

set(RVSW_MARCH "rv32ima_zicsr_zifencei")#Your -march=[...] here
set(RVSW_MABI "ilp32")#Your -mabi=[...] here

set(RVSW_EXIT_METHOD "SELF_JUMP")#One of: SELF_JUMP, CUSTOM_INSTRUCTION, FUNCTION_CALL, or potentially others in the future
set(RVSW_CUSTOM_EXIT_INSTR ".insn r CUSTOM_0, 0, 0, zero, zero, zero")#Put your custom exit instruction here (this is the one from IRVE for example)
set(RVSW_CUSTOM_EXIT_FN "my_custom_exit_fn_symbol")

#set(RVSW_MMODE_NEWLIB_SYSCALLS_STATICLIB_CMAKE_TARGET "my_newlib_syscalls_staticlib")#The cmake target for your newlib syscalls static library
set(RVSW_MMODE_NEWLIB_SYSCALLS_STATICLIB_CMAKE_TARGET "c")#Just use "c" here if you don't want to supply syscalls

#Your own firmware must link against the SBI static library that is built
set(RVSW_BUILD_SBI "ogsbi")#Uncomment if you want to build an SBI (and specify the name of the one you want to build)

#TODO options for firmware

set(RVSW_BUILD_SMODE_SW "1")#Set to 1 if you want to build supervisor mode software too
set(RVSW_SMODE_LD_SCRIPT "${PROJECT_SOURCE_DIR}/example_linker_script.ld")#Your linker script here (relative to the root of the rvsw checkout) (can be different than smode)
set(RVSW_STVEC_IS_HARDWIRED "VECTORED")#Uncomment if not hardwired, else "DIRECT" if direct, or "VECTORED" if vectored
set(RVSW_SMODE_DATA_SEGMENT_ALREADY_LOADED "1")#Set to 0 if you need the C startup code to load the data segment before calling anything
set(RVSW_SMODE_BSS_SEGMENT_ALREADY_ZEROED "0")#Set to 0 if you need the C startup code to zero the bss segment before calling anything
set(RVSW_SMODE_MARCH "rv32ima_zicsr_zifencei")#Your -march=[...] here (can be different from M-mode; ex if mmode code is emulating certain instructions)
set(RVSW_SMODE_MABI "ilp32")#Your -mabi=[...] here (can be different from M-mode)
