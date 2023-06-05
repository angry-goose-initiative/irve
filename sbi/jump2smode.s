# jump2smode.s
# Copyright (C) 2023 John Jekel and Nick Chan
# See the LICENSE file at the root of the project for licensing info.
#
# Entry point back into M mode upon recieving an ecall
#

.global jump2smode
jump2smode:#Takes one argument, the address to jump to in S-Mode
    #Preserve the M-Mode stack pointer in mscratch
    csrw mscratch, sp

    #Set mstatus so that when we execute mret, we will go to S-Mode
    li t0, 0b00000000000000000001100000000000
    li t1, 0b00000000000000000000100000000000
    csrc mstatus, t0#Clear MPP
    csrs mstatus, t1#Set MPP to 0b01 (S-Mode)

    #Set mepc to the address contained within a0
    csrw mepc, a0

    #Go to S-Mode
    mret

    #We should never return. Leave this as a sanity check so we exit if we do.
    .insn r CUSTOM_0, 0, 0, zero, zero, zero
