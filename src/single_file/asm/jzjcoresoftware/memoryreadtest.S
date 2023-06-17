#include "rvsw_asm.h"

.section .reset, "ax"
.global ___rvsw_reset___
___rvsw_reset___:
    lw x31, 4(x0)
    .word 2863311530
