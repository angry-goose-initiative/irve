#include "rvsw_asm.h"

.section .init, "ax"
.global ___rvsw_reset___
___rvsw_reset___:
    addi x31, x31, 2
    jalr x0, x0, 0
    addi x31, x31, 2
    RVSW_EXIT_ASM
