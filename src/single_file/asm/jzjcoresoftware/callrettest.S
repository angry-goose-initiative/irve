#include "rvsw_asm.h"

entry:
    addi x31, x31, 10
    call functiontest
    RVSW_EXIT_ASM
    
functiontest:
    addi x31, x31, 17
    ret
