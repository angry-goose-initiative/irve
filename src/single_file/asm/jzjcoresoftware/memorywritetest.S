#include "rvsw_asm.h"

    addi x30, x0, 26
    sw x30, 16(x0)
    lw x31, 16(x0)
    RVSW_EXIT_ASM
