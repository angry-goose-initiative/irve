#include "rvsw_asm.h"

.section .reset, "ax"
.global ___rvsw_reset___
___rvsw_reset___:
    lui x31, 703710
    RVSW_EXIT_ASM
