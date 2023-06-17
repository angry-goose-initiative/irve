#include "rvsw_asm.h"

.section .init, "ax"
.global ___rvsw_reset___
___rvsw_reset___:
    addi x31, x31, 10
    call functiontest
    RVSW_EXIT_ASM
    
functiontest:
    addi x31, x31, 17
    ret
