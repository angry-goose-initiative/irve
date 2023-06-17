# vector_table.S
# Copyright (C) 2023 John Jekel and Nick Chan
# See the LICENSE file at the root of the project for licensing info.
#
# RVSW vector table and entry point for RISC-V code.
#

#TODO use ___ (to the ends as well) to prevent collisions

#include "rvsw_asm.h"

################################################################################################################
# Entry point
################################################################################################################

.section .init, "ax"
.weak ___rvsw_reset___#Assembly entry point
___rvsw_reset___:
    j ___rvsw_crt0___#It is important this is exactly 4 bytes long to avoid interfering with the vector table

################################################################################################################
# Vector table
################################################################################################################
.section .vector_table, "ax"
    j   __riscv_synchronous_exception_and_user_mode_swi_handler #Address: 0x00000004#FIXME the RISC-V spec is kind of confusing on what should go here...
    j   __riscv_supervisor_software_interrupt_handler           #Address: 0x00000008#Perhaps this should go to 0 too?
    .word 0x00000000#Reserved                                   #Address: 0x0000000C
    j   __riscv_machine_software_interrupt_handler              #Address: 0x00000010#Perhaps this should go to 0 too?
    .word 0x00000000#Reserved                                   #Address: 0x00000014
    j   __riscv_supervisor_timer_interrupt_handler              #Address: 0x00000018
    .word 0x00000000#Reserved                                   #Address: 0x0000001C
    j   __riscv_machine_timer_interrupt_handler                 #Address: 0x00000020
    .word 0x00000000#Reserved                                   #Address: 0x00000024
    j   __riscv_supervisor_external_interrupt_handler           #Address: 0x00000028
    .word 0x00000000#Reserved                                   #Address: 0x0000002C
    j   __riscv_machine_external_interrupt_handler              #Address: 0x00000030

    #TODO custom ones here

################################################################################################################
# Weak symbols for the exception/interrupt handlers and the default handler
################################################################################################################
.section .text, "ax"
.global __default_riscv_exception_and_interrupt_handler
default_riscv_exception_and_interrupt_handler:#Called when a trap or interrupt occurs EXCEPT for reset by default

.weak __riscv_synchronous_exception_and_user_mode_swi_handler
__riscv_synchronous_exception_and_user_mode_swi_handler:#For all synchronous exceptions and the user mode software interrupt (NOT other interrupts)

.weak __riscv_supervisor_software_interrupt_handler
__riscv_supervisor_software_interrupt_handler:#For the supervisor software interrupt

.weak __riscv_machine_software_interrupt_handler
__riscv_machine_software_interrupt_handler:#For the machine software interrupt

.weak __riscv_supervisor_timer_interrupt_handler
__riscv_supervisor_timer_interrupt_handler:#For the supervisor timer interrupt

.weak __riscv_machine_timer_interrupt_handler
__riscv_machine_timer_interrupt_handler:#For the machine timer interrupt

.weak __riscv_supervisor_external_interrupt_handler
__riscv_supervisor_external_interrupt_handler:#For the supervisor external interrupt

.weak __riscv_machine_external_interrupt_handler
__riscv_machine_external_interrupt_handler:#For the machine external interrupt

#By default, if there is no interrupt handler, halt the cpu
    RVSW_EXIT_ASM
