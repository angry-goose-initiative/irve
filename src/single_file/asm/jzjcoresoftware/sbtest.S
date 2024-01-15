# Copyright (C) 2020-2024 John Jekel
#See the LICENSE file at the root of the project for licensing info
#include "rvsw_asm.h"

.section .reset, "ax"
.global ___rvsw_reset___
___rvsw_reset___:
    sb x0, 12(x0)
    lw x31, 12(x0)
    RVSW_EXIT_ASM

    .word 0x89abcdef
    #The assembler will write the above data into memory as 0xefcdab89 starting at address 0x12
    #sb will overwrite part of the data and turn it to 0x00cdab89
    #at the end x31 should be loaded with 0x89abcd00
