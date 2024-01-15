# gp_tp_preserve_restore.s
# Copyright (C) 2023-2024 John Jekel
# See the LICENSE file at the root of the project for licensing info.
#
# Code for preserving and restoring the gp and tp before going to/
# after returning from S-Mode.
# 
# NOTE: The program MUST be compiled with -fomit-frame-pointer since we do not save that.
#

#NOTE: WE EXPLICITLY USE ABSOLUTE ADDRESSING TO AVOID USING GP TO ACCESS THE GLOBAL VARIABLE PRESERVING IT
#This avoids a chicken and egg problem where we need to restore gp to access the global variable preserving gp

.global preserve_mmode_gp_tp
preserve_mmode_gp_tp:
    lui t0, %hi(mmode_preserved_gp)
    addi t0, t0, %lo(mmode_preserved_gp)
    lui t1, %hi(mmode_preserved_tp)
    addi t1, t1, %lo(mmode_preserved_tp)
    sw gp, 0(t0)
    sw tp, 0(t1)
    ret

.global restore_mmode_gp_tp
restore_mmode_gp_tp:
    lui t0, %hi(mmode_preserved_gp)
    addi t0, t0, %lo(mmode_preserved_gp)
    lui t1, %hi(mmode_preserved_tp)
    addi t1, t1, %lo(mmode_preserved_tp)
    lw gp, 0(t0)
    lw tp, 0(t1)
    ret

.data
mmode_preserved_gp:
.word 0xBADDCAFE
mmode_preserved_tp:
.word 0xBADDCAFE
