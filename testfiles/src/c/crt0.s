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
.global _start
_start:
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

    #Call Newlib's init
    #TODO is this the proper way to do this?
    jal x1, __libc_init_array
    
    #Jump to main
    jal x1, main
    
    #Halt cpu if we ever return from main (using ebreak instead of ecall like JZJCores did)
    #TODO we should be calling "destructors" (the c atexit() function) before we halt
    ebreak
    
    #Hint to the assembler about the end of the function
    .cfi_endproc
    
    #Hint to assembler about end of file
    .end
