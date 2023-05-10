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

#Practically identical to https://twilco.github.io/riscv-from-scratch/2019/04/27/riscv-from-scratch-2.html
.section .init, "ax"
.weak _start
_start:#NOTE: This is both the reset entry point, and also the hardcoded MTVEC address
    #Hint to assembler about start of function
    .cfi_startproc
    .cfi_undefined ra

    #Check mepc for exception reason (don't modify it)
    csrrs t0, mcause, zero

    #If it was for any reason other than reset, jump to the interrupt handler
    li t1, 24
    bne t0, t1, __interrupt_and_trap_handler

    #If we got here, we are booting up for the first time
    
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

    #Halt cpu if we ever return from main (using a custom instruction and falling through the next function symbol to save 4 bytes of memory)
.weak __interrupt_and_trap_handler#By default, if there is no interrupt handler, halt the cpu
__interrupt_and_trap_handler:#Called when a trap or interrupt occurs EXCEPT for reset
    .insn r CUSTOM_0, 0, 0, zero, zero, zero
    
    #Hint to the assembler about the end of the function
    .cfi_endproc
