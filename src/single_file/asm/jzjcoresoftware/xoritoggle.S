#include "rvsw_asm.h"

.section .reset, "ax"
.global ___rvsw_reset___
___rvsw_reset___:
    #Because of sign extention, the xori instruction below xor's all of the bits of x31 with 1 (effectively inverting them)
    xori x31, x31, -1
    
    #jump back to the start
    jalr x0, x0, 0
