# interrupt_entry.s
# Copyright (C) 2024 John Jekel
# See the LICENSE file at the root of the project for licensing info.
#
# Entry point back into M mode upon recieving an interrupt
# 
# NOTE: This cannot just be in C because the compiler just pushes registers
# to the stack, but we need to ensure this is to the bootloader stack rather
# than the supervisor or userspace stack
# 
# NOTE: We delegate all S-Mode interrupts to S-Mode, so only M-Mode interrupts need be handled here
#
# NOTE: Unlike exceptions where we can print things out for debugging, we can't do that here since
# print functions may not be re-entrant, and we're already using them for exceptions. Yes this 
# shouldn't matter since a trap to m-mode would disable interrupts, but it's better to be safe than sorry.
#

#TODO support DIRECT mode too (not just vectored)

#include "rvsw_asm.h"

.global ___rvsw_machine_software_interrupt_handler___
___rvsw_machine_software_interrupt_handler___:
    #Not implemented since only OGSBI can set the software interrupt pending bit, but we don't use
    #it for anything (yet)
    RVSW_EXIT_ASM

.global ___rvsw_machine_timer_interrupt_handler___
___rvsw_machine_timer_interrupt_handler___:
    #Restore the M-mode stack pointer and save the S-mode one (swap with mscratch)
    csrrw sp, mscratch, sp

    #Push S-mode t0 to the M-mode stack
    addi sp, sp, -4
    sw t0, 0(sp)

    #We simply set the supervisor timer interrupt pending bit and return, thereby passing
    #the timer interrupt to the supervisor transparently since hardware only causes m-mode interrupts
    li t0, (1 << 5)
    csrs mip, t0//Set the supervisor timer interrupt pending bit
    li t0, (1 << 7)
    csrc mip, t0//Clear the machine timer interrupt pending bit

    #Pop S-mode t0 from the M-mode stack
    lw t0, 0(sp)
    addi sp, sp, 4

    #Restore the S-mode stack pointer and save the M-mode one (swap with mscratch)
    csrrw sp, mscratch, sp

    mret//Returning will re-enable interrupts and jump to the supervisor's timer interrupt handler

.global ___rvsw_machine_external_interrupt_handler___
___rvsw_machine_external_interrupt_handler___:
    #Restore the M-mode stack pointer and save the S-mode one (swap with mscratch)
    csrrw sp, mscratch, sp

    #Push S-mode t0 to the M-mode stack
    addi sp, sp, -4
    sw t0, 0(sp)

    #We simply set the supervisor external interrupt pending bit and return, thereby passing
    #the external interrupt to the supervisor transparently since hardware only causes m-mode interrupts
    li t0, (1 << 9)
    csrs mip, t0//Set the supervisor external interrupt pending bit
    li t0, (1 << 11)
    csrc mip, t0//Clear the machine external interrupt pending bit

    #Pop S-mode t0 from the M-mode stack
    lw t0, 0(sp)
    addi sp, sp, 4

    #Restore the S-mode stack pointer and save the M-mode one (swap with mscratch)
    csrrw sp, mscratch, sp

    mret//Returning will re-enable interrupts and jump to the supervisor's external interrupt handler
