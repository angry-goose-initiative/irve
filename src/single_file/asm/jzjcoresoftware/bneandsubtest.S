# Copyright (C) 2020-2024 John Jekel
#See the LICENSE file at the root of the project for licensing info
#include "rvsw_asm.h"

.section .reset, "ax"
.global ___rvsw_reset___
___rvsw_reset___:
    addi x31, x0, 1000
    addi x30, x0, 500
    addi x29, x0, 5
    
    #loop
    #subtract 5
    #addi x31, x31, -5
    sub x31, x31, x29
    
    #exit when x31 reaches 500
    #bne x31, x30, -4
    #assembler is changing bne to a beq and a jal so I'm just going to hand assemble this (same as above instruction)
    .word 0xffef9ee3
    RVSW_EXIT_ASM
