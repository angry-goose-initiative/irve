#include "rvsw_asm.h"

.section .init, "ax"
.global ___rvsw_reset___
___rvsw_reset___:
    nop
    nop
    nop
    auipc x31, 1
    RVSW_EXIT_ASM
