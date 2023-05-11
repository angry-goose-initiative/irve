# crt0.s
# Copyright (C) 2023 John Jekel and Nick Chan
# See the LICENSE file at the root of the project for licensing info.
#
# Bootstrap code that sets up a minimal C environment
#
# Based on code from jzjcoresoftware, which used the following websites to create this:
#
# https://twilco.github.io/riscv-from-scratch/2019/04/27/riscv-from-scratch-2.html
# https://stackoverflow.com/questions/50214840/risc-v-startup-code-for-stack-pointer-initalization
# https://gnu-mcu-eclipse.github.io/arch/riscv/programmer/
# https://embarc.org/man-pages/as/RISC_002dV_002dDirectives.html
# https://www.reddit.com/r/RISCV/comments/enmqdz/linker_script_fault/
# https://github.com/deadsy/rvemu/blob/master/test/test1/emu.ld
# https://stackoverflow.com/questions/57021029/how-to-link-math-library-when-building-bare-metal-application-with-custom-linker
# Practically identical to https://twilco.github.io/riscv-from-scratch/2019/04/27/riscv-from-scratch-2.html

.section .init, "ax"
.weak __crt0#C Runtime Initialization (0th stage)
__crt0:
    #Hint to assembler about start of function
    .cfi_startproc
    .cfi_undefined ra

    #Setup global pointer
    .option push
    .option norelax
    la gp, __global_pointer$
    .option pop
    
    #Setup stack pointer based on linker script symbol
    la sp, __stack_top

    #Allow code to (ex. setup Newlib or anything else) to run before main
    jal ra, __pre_main
.weak __pre_main#By default, do nothing, and just jump to the next instruction
__pre_main:
    
    #Jump to main
    jal ra, main

    #Allow code to (ex. teardown Newlib or anything else) to run after main
    jal ra, __post_main
.weak __post_main#By default, do nothing, and just jump to the next instruction
__post_main:

    #Halt cpu if we ever return from main
    .insn r CUSTOM_0, 0, 0, zero, zero, zero
    
    #Hint to the assembler about the end of the function
    .cfi_endproc

    #Hint to assembler about end of file
    .end
