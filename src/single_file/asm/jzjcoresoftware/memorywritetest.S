# Copyright (C) 2020-2024 John Jekel
#See the LICENSE file at the root of the project for licensing info
#include "rvsw_asm.h"

.section .reset, "ax"
.global ___rvsw_reset___
___rvsw_reset___:
    addi x30, x0, 26
    sw x30, 16(x0)
    lw x31, 16(x0)
    RVSW_EXIT_ASM
