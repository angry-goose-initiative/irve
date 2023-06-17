# cinit.S
# Copyright (C) 2023 John Jekel and Nick Chan
# See the LICENSE file at the root of the project for licensing info.
#
# Bootstrap code that sets up a minimal C environment
#
# Based on code from irve, which used the following websites to create this:
#
# https://twilco.github.io/riscv-from-scratch/2019/04/27/riscv-from-scratch-2.html
# https://stackoverflow.com/questions/50214840/risc-v-startup-code-for-stack-pointer-initalization
# https://gnu-mcu-eclipse.github.io/arch/riscv/programmer/
# https://embarc.org/man-pages/as/RISC_002dV_002dDirectives.html
# https://www.reddit.com/r/RISCV/comments/enmqdz/linker_script_fault/
# https://github.com/deadsy/rvemu/blob/master/test/test1/emu.ld
# https://stackoverflow.com/questions/57021029/how-to-link-math-library-when-building-bare-metal-application-with-custom-linker
# Practically identical to https://twilco.github.io/riscv-from-scratch/2019/04/27/riscv-from-scratch-2.html

#include "rvsw_asm.h"

.section .text, "ax"
.weak ___rvsw_crt0___#C Runtime Initialization (0th stage)
___rvsw_crt0___:
    #Hint to assembler about start of function
    .cfi_startproc
    .cfi_undefined ra

    #Setup global pointer
    .option push
    .option norelax
    la gp, ___rvsw_global_pointer___$
    .option pop
    
    #Setup stack pointer based on linker script symbol
    la sp, ___rvsw_stack_top___

    #Allow code to (ex. setup Newlib or anything else) to run before main
    jal ra, ___rvsw_pre_main___
.weak ___rvsw_pre_main___#By default, do nothing, and just jump to the next instruction
___rvsw_pre_main___:
    
    #Jump to main
    jal ra, main

    #Allow code to (ex. teardown Newlib or anything else) to run after main
    jal ra, ___rvsw_post_main___
.weak ___rvsw_post_main___#By default, do nothing, and just jump to the next instruction
___rvsw_post_main___:

    #Halt cpu if we ever return from main
    RVSW_EXIT_ASM
    
    #Hint to the assembler about the end of the function
    .cfi_endproc

    #Hint to assembler about end of file
    .end
