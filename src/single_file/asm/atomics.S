# atomics.S
# Copyright (C) 2023 John Jekel and Nick Chan
# See the LICENSE file at the root of the project for licensing info.
#
# Testing atomic instructions
#

#include "rvsw_asm.h"

.section .reset, "ax"
.global ___rvsw_reset___#Assembly entry point
___rvsw_reset___:
    li x1 0x1
end:
    RVSW_EXIT_ASM
