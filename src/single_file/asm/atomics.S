# atomics.S
# Copyright (C) 2023 Nick Chan
# See the LICENSE file at the root of the project for licensing info.
#
# Testing atomic instructions
#

#include "rvsw_asm.h"

.section .reset, "ax"
.global ___rvsw_reset___ # Assembly entry point
___rvsw_reset___:
.local my_words
    // a0 holds the address of my_words
    la a0, my_words
    lw t1, 0(a0)
    
    // Testing load reserved and store conditional

    lr.w t0, 0(a0)
    lw t0, 4(a0) // new value to store

    sc.w t1, t0, 0(a0) // this should succeed (t1 == 0)
    sc.w t1, t0, 0(a0) // this should fail (t1 == 1)

    lw t1, 0(a0)

    // Testing amo

    lw t0, 8(a0)
    sw t0, 0(a0)
    lw t0, 4(a0)
    amoswap.w t1, t0, 0(a0)
    lw t2, 0(a0)

    lw t0, 12(a0)
    sw t0, 0(a0)
    lw t0, 16(a0)
    amoadd.w t1, t0, 0(a0)
    lw t2, 0(a0)

    lw t0, 12(a0)
    sw t0, 0(a0)
    lw t0, 16(a0)
    amoand.w t1, t0, 0(a0)
    lw t2, 0(a0)

    lw t0, 12(a0)
    sw t0, 0(a0)
    lw t0, 16(a0)
    amoor.w t1, t0, 0(a0)
    lw t2, 0(a0)

    lw t0, 12(a0)
    sw t0, 0(a0)
    lw t0, 16(a0)
    amoxor.w t1, t0, 0(a0)
    lw t2, 0(a0)

    lw t0, 20(a0)
    sw t0, 0(a0)
    lw t0, 24(a0)
    amomax.w t1, t0, 0(a0)
    lw t2, 0(a0)

    lw t0, 20(a0)
    sw t0, 0(a0)
    lw t0, 24(a0)
    amomaxu.w t1, t0, 0(a0)
    lw t2, 0(a0)

    lw t0, 20(a0)
    sw t0, 0(a0)
    lw t0, 24(a0)
    amomin.w t1, t0, 0(a0)
    lw t2, 0(a0)

    lw t0, 20(a0)
    sw t0, 0(a0)
    lw t0, 24(a0)
    amominu.w t1, t0, 0(a0)
    lw t2, 0(a0)

end:
    RVSW_EXIT_ASM

.data
my_words:
.word 0x12345678
.word 0x87654321
.word 0xABCDEF01
.word 0x11111111
.word 0x33333333
.word 0xFFFFFFFF
.word 0x7FFFFFFF
