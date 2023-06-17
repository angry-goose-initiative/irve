# user_and_back_in_time_for_dinner.s
# Copyright (C) 2023 John Jekel and Nick Chan
# See the LICENSE file at the root of the project for licensing info.
#
# Goin' all the way to U-Mode and back to M-Mode!
#

#include "rvsw_asm.h"

.section .reset, "ax"
.global ___rvsw_reset___#Assembly entry point
___rvsw_reset___:
    j mmode_entry

.section .vector_table, "ax"
    j mmode_ecall_handler
    RVSW_EXIT_ASM
    RVSW_EXIT_ASM
    RVSW_EXIT_ASM
    RVSW_EXIT_ASM
    RVSW_EXIT_ASM
    RVSW_EXIT_ASM
    RVSW_EXIT_ASM
    RVSW_EXIT_ASM
    RVSW_EXIT_ASM
    RVSW_EXIT_ASM
    RVSW_EXIT_ASM

.text
mmode_entry:
    la a0, mmode_string0
    call print

    csrrw zero, medeleg, zero#All exceptions are handled in M-Mode

    #Set mstatus so that when we execute mret, we will go to S-Mode
    li t0, 0b00000000000000000001100000000000
    li t1, 0b00000000000000000000100000000000
    csrrc zero, mstatus, t0#Clear MPP
    csrrs zero, mstatus, t1#Set MPP to 0b01 (S-Mode)

    #Set mepc to the address of the smode_entry label
    la t0, smode_entry
    csrrw zero, mepc, t0

    #Go to S-Mode
    mret

mmode_ecall_handler:
    la a0, mmode_string1
    call print
    RVSW_EXIT_ASM

smode_entry:
    la a0, smode_string
    call print

    #Set sstatus so that when we execute sret, we will go to U-Mode
    li t0, 0b00000000000000000000000100000000
    csrrc zero, sstatus, t0#Set SPP to 0b0 (U-Mode)

    #Set sepc to the address of the umode_entry label
    la t0, umode_entry
    csrrw zero, sepc, t0

    #Go to U-Mode
    sret

umode_entry:
    la a0, umode_string
    call print
    ecall

print:#Expect address in a0
    lbu t0, 0(a0)
    beqz t0, 1f
    sb t0, -1(zero)
    addi a0, a0, 1
    j print
1:
    ret

mmode_string0:
.string "Hello from M-Mode!\n"
    
mmode_string1:
.string "Hello from M-Mode again!\n"

smode_string:
.string "Hello from S-Mode!\n"

umode_string:
.string "Hello from U-Mode!\n"
