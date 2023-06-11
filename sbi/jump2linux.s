# jump2linux.s
# Copyright (C) 2023 John Jekel and Nick Chan
# See the LICENSE file at the root of the project for licensing info.
#
# Starts the Linux kernel in S-Mode
# 

.global jump2linux
jump2linux:#Arguments: a0 = hart ID, a1 = address of device tree blob, a2 = address of kernel image
    #Set mstatus so that when we execute mret, we will go to S-Mode
    li t0, 0b00000000000000000001100000000000
    li t1, 0b00000000000000000000100000000000
    csrc mstatus, t0#Clear MPP
    csrs mstatus, t1#Set MPP to 0b01 (S-Mode)

    #Set mepc to the address contained within a2
    csrw mepc, a2

    #Preserve the M-Mode stack pointer in mscratch
    csrw mscratch, sp

    #Preserve the M-Mode global and thread pointers (pc-relative so this should work)
    la t0, mmode_preserved_gp
    la t1, mmode_preserved_tp
    sw gp, 0(t0)
    sw tp, 0(t1)

    #NOTE: There is no point in saving any other registers since this function is noreturn. So we needn't
    #preserve any callee-saved registers.

    #For security, zero all registers other than a0 and a1
    #The kernel will look at a0 and a1. We only needed a2 to set mepc, so we can zero it too.
    #FIXME why does zeroing these prevent us from printing from exceptions later on?
    li x1, 0
    li x2, 0
    li x3, 0
    li x4, 0
    li x5, 0
    li x6, 0
    li x7, 0
    li x8, 0
    li x9, 0
    #NOT x10 (a0)
    #NOT x11 (a1)
    li x12, 0
    li x13, 0
    li x14, 0
    li x15, 0
    li x16, 0
    li x17, 0
    li x18, 0
    li x19, 0
    li x20, 0
    li x21, 0
    li x22, 0
    li x23, 0
    li x24, 0
    li x25, 0
    li x26, 0
    li x27, 0
    li x28, 0
    li x29, 0
    li x30, 0
    li x31, 0

    #The kernel expects the hart ID and address of the DTB in a0 and a1, so we don't have to do anything else!

    #Go to S-Mode (jumps to address in mepc)
    mret

    #We should never return. Leave this as a sanity check so we exit if we do.
    .insn r CUSTOM_0, 0, 0, zero, zero, zero
