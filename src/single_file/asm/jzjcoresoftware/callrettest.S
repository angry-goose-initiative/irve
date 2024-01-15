# Copyright (C) 2020-2024 John Jekel
#See the LICENSE file at the root of the project for licensing info
#include "rvsw_asm.h"

.section .reset, "ax"
.global ___rvsw_reset___
___rvsw_reset___:
    addi x31, x31, 10
    call functiontest
    RVSW_EXIT_ASM
    
functiontest:
    addi x31, x31, 17
    ret
