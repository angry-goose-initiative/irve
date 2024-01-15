# Copyright (C) 2020-2024 John Jekel
#See the LICENSE file at the root of the project for licensing info
#include "rvsw_asm.h"

.section .reset, "ax"
.global ___rvsw_reset___
___rvsw_reset___:
    #Register setup
    #shift amount register (always 1)
    addi x28, x0, 1
    #loop i register
    addi x29, x0, 0
    #loop comparison register (always 15)
    addi x30, x0, 15
    #Data output
    addi x31, x0, 1
    
    #Main Loop
    #Sub loop 1 (shifting left)
    sll x31, x31, x28
    addi x29, x29, 1
    #blt x29, x30, -8
    .word 0xffeecce3
    #In between loops (reset i register)
    addi x29, x0, 0
    #Sub loop 2 (shifting right)
    srl x31, x31, x28
    addi x29, x29, 1
    #blt x29, x30, -8
    .word 0xffeecce3
    #Reset stuffs and return to start of main loop
    addi x29, x0, 0
    jalr x0, x0, 16
    
    #Note: blts are not playing nice; assembler converts them to a bge and a j that just loop back to itself forever, so I've hand assembled them instead
