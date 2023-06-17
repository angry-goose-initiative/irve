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
.weak ___rvsw_crt0___#C Runtime Initialization
___rvsw_crt0___:
    #Hint to assembler about start of function
    .cfi_startproc
    .cfi_undefined ra

    #Setup global pointer. We must do it this way to avoid using the gp before we actually have it
    .option push
    .option norelax
    la gp, ___rvsw_global_pointer___$
    .option pop
    
    #Setup stack pointer based on linker script symbol
    la sp, ___rvsw_stack_top___

    #Zero out the BSS section if needed
#if RVSW_NEED_BSS_ZEROED
    la t0, ___rvsw_bss_start___
    la t1, ___rvsw_bss_end___
0: #BSS-zeroing loop
    bgtu t0, t1, 1f
    sb zero, 0(t0)
    addi t0, t0, 1
    j 0b
1:  #End of BSS-zeroing loop
#endif

    #Load the data segment into memory if needed
#if RVSW_NEED_DATA_LOADED
    la t0, ___rvsw_data_rom_start___
    la t1, ___rvsw_data_rom_end___
    la t2, ___rvsw_data_ram_start___
0: #Data-loading loop
    bgtu t0, t1, 1f
    lb t3, 0(t0)
    sb t3, 0(t2)
    addi t0, t0, 1
    addi t2, t2, 1
    j 0b
1:  #End of data-loading loop
#endif

    #Setup Newlib code
    call __libc_init_array
    
    #Jump to main
    call main

    #Post-main RVSW-specific cleanup in C
    call ___rvsw_cleanup___

    #Teardown Newlib code
    call __libc_fini_array

    #Halt cpu if we ever return from main
    RVSW_EXIT_ASM
    
    #Hint to the assembler about the end of the function
    .cfi_endproc

    #Hint to assembler about end of file
    .end
