# atomics.S
# Copyright (C) 2023 John Jekel and Nick Chan
# See the LICENSE file at the root of the project for licensing info.
#
# Testing atomic instructions
#

#include "rvsw_asm.h"

.section .reset, "ax"
.global ___rvsw_reset___ # Assembly entry point
___rvsw_reset___:
    addi a0, zero, 0x00000100 // address
    addi a1, zero, 0x00000123 // value to store
    sw a1, (a0)
    
    lr.w t0, (a0) // load the value that was just stored
    addi t2, zero, 0x00000321 // new value to store

    sc.w t1, t2, (a0) // this should succeed (t1 == 0)
    sc.w t1, t2, (a0) // this should fail (t1 == 1)
end:
    RVSW_EXIT_ASM
