# jump2linux.s
# Copyright (C) 2023 John Jekel and Nick Chan
# See the LICENSE file at the root of the project for licensing info.
#
# Starts the Linux kernel in S-Mode
# 

.global jump2linux
jump2linux:#Arguments: a0 = hart ID, a1 = address of device tree blob, a2 = address of kernel image
    #Preserve the M-Mode stack pointer in mscratch
    csrw mscratch, sp

    #Preserve the M-Mode global and thread pointers so C and assembly code can access global variables (they will need to be restored later)
    call preserve_mmode_gp_tp

    #NOTE: There is no point in saving any other registers since this function is noreturn. So we needn't
    #preserve any callee-saved registers for example, let alone any other registers.

    #Set mstatus so that when we execute mret, we will go to S-Mode
    li t0, 0b00000000000000000001100000000000
    li t1, 0b00000000000000000000100000000000
    csrc mstatus, t0#Clear MPP
    csrs mstatus, t1#Set MPP to 0b01 (S-Mode)

    #Set mepc to the address contained within a2
    csrw mepc, a2

    #For security and better debugging, clobber all registers other than a0 and a1
    #The kernel will look at a0 and a1. We only needed a2 to set mepc, so we can zero it too.
    li x1, 0xBADBADBA
    li x2, 0xBADBADBA
    li x3, 0xBADBADBA
    li x4, 0xBADBADBA
    li x5, 0xBADBADBA
    li x6, 0xBADBADBA
    li x7, 0xBADBADBA
    li x8, 0xBADBADBA
    li x9, 0xBADBADBA
    #NOT x10 (a0)
    #NOT x11 (a1)
    li x12, 0xBADBADBA
    li x13, 0xBADBADBA
    li x14, 0xBADBADBA
    li x15, 0xBADBADBA
    li x16, 0xBADBADBA
    li x17, 0xBADBADBA
    li x18, 0xBADBADBA
    li x19, 0xBADBADBA
    li x20, 0xBADBADBA
    li x21, 0xBADBADBA
    li x22, 0xBADBADBA
    li x23, 0xBADBADBA
    li x24, 0xBADBADBA
    li x25, 0xBADBADBA
    li x26, 0xBADBADBA
    li x27, 0xBADBADBA
    li x28, 0xBADBADBA
    li x29, 0xBADBADBA
    li x30, 0xBADBADBA
    li x31, 0xBADBADBA

    #The kernel expects the hart ID and address of the DTB in a0 and a1, so we don't have to do anything else!

    #Go to S-Mode (jumps to address in mepc)
    mret

    #We should never return. Leave this as a sanity check so we exit if we do.
    .insn r CUSTOM_0, 0, 0, zero, zero, zero
