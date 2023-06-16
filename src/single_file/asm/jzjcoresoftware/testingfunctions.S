#include "rvsw_asm.h"

entry:
    addi x31, x0, 10
    jal x1, functiontest
    RVSW_EXIT_ASM
    
functiontest:
    addi x31, x31, 17
    jalr x0, x1, 0
