# vector_table.S
# Copyright (C) 2023-2024 John Jekel
# See the LICENSE file at the root of the project for licensing info.
#
# RVSW vector table and entry point for RISC-V code.
#

#include "rvsw_asm.h"

################################################################################################################
# Entry point
################################################################################################################

.section .reset, "ax"
.global ___rvsw_reset___#Assembly entry point
___rvsw_reset___:
    j ___rvsw_crt0___#It is important this is exactly 4 bytes long to avoid interfering with the vector table

################################################################################################################
# Vector table
################################################################################################################

.section .vector_table, "ax"

.global ___rvsw_vector_table___
___rvsw_vector_table___:
#if RVSW_VECTORED_ABRUPT_CONTROL_FLOW
    j   ___rvsw_exception_handler___
    j   ___rvsw_supervisor_software_interrupt_handler___
    .word 0x00000000#Reserved
    j   ___rvsw_machine_software_interrupt_handler___
    .word 0x00000000#Reserved
    j   ___rvsw_supervisor_timer_interrupt_handler___
    .word 0x00000000#Reserved
    j   ___rvsw_machine_timer_interrupt_handler___
    .word 0x00000000#Reserved
    j   ___rvsw_supervisor_external_interrupt_handler___
    .word 0x00000000#Reserved
    j   ___rvsw_machine_external_interrupt_handler___
    #TODO custom ones here
#else
    #The hardware does not vectored control-flow changes
    j ___rvsw_direct_trap_handler___
#endif

################################################################################################################
# Weak symbols for the exception/interrupt handlers and the default handler
################################################################################################################

.section .text, "ax"

.global ___rvsw_default_exception_and_interrupt_handler___
___rvsw_default_exception_and_interrupt_handler___:#Called when a trap or interrupt occurs EXCEPT for reset by default

#if RVSW_VECTORED_ABRUPT_CONTROL_FLOW
.weak ___rvsw_exception_handler___
___rvsw_exception_handler___:#For all synchronous exceptions and the user mode software interrupt (NOT other interrupts)

.weak ___rvsw_supervisor_software_interrupt_handler___
___rvsw_supervisor_software_interrupt_handler___:#For the supervisor software interrupt

.weak ___rvsw_machine_software_interrupt_handler___
___rvsw_machine_software_interrupt_handler___:#For the machine software interrupt

.weak ___rvsw_supervisor_timer_interrupt_handler___
___rvsw_supervisor_timer_interrupt_handler___:#For the supervisor timer interrupt

.weak ___rvsw_machine_timer_interrupt_handler___
___rvsw_machine_timer_interrupt_handler___:#For the machine timer interrupt

.weak ___rvsw_supervisor_external_interrupt_handler___
___rvsw_supervisor_external_interrupt_handler___:#For the supervisor external interrupt

.weak ___rvsw_machine_external_interrupt_handler___
___rvsw_machine_external_interrupt_handler___:#For the machine external interrupt
#else
#The hardware does not vectored control-flow changes
.weak ___rvsw_direct_trap_handler___
___rvsw_direct_trap_handler___:
#endif

#By default, if there is no interrupt handler, halt the cpu
    RVSW_EXIT_ASM

