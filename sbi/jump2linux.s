# jump2linux.s
# Copyright (C) 2023 John Jekel and Nick Chan
# See the LICENSE file at the root of the project for licensing info.
#
# Starts the Linux kernel in S-Mode
# 

.global jump2linux
jump2linux:#Arguments: a0 = hart ID, a1 = address of device tree blob, a2 = address of kernel image
    #TODO preserve M-Mode registers other than SP on the M-Mode stack

    #TODO for security should we also clear registers other than a0 and a1?

    #Preserve the M-Mode stack pointer in mscratch
    csrw mscratch, sp

    #Set mstatus so that when we execute mret, we will go to S-Mode
    li t0, 0b00000000000000000001100000000000
    li t1, 0b00000000000000000000100000000000
    csrc mstatus, t0#Clear MPP
    csrs mstatus, t1#Set MPP to 0b01 (S-Mode)

    #Set mepc to the address contained within a2
    csrw mepc, a2

    #TESTING
    #li t0, 'A'
    #sb t0, -1(zero)
    #li t0, '\n'
    #sb t0, -1(zero)

    #The kernel expects the hart ID and address of the DTB in a0 and a1, so we don't have to do anything else!

    #Go to S-Mode
    mret

    #We should never return. Leave this as a sanity check so we exit if we do.
    .insn r CUSTOM_0, 0, 0, zero, zero, zero
