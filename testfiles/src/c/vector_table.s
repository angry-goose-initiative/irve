# vector_table.s
# Copyright (C) 2023 John Jekel and Nick Chan
# See the LICENSE file at the root of the project for licensing info.
#
# IRVE vector table and entry point for RISC-V code.
# NOTE: For IRVE, the PC is always initialized to 0x00000000 at startup for
# backwards compatibility with raw assembly (and it also makes it a ton easier to write even new standalone assembly)
#
# MTVEC is hardcoded such the the vector table starts at 0x00000004
# So address 0x00000000 for the entry point effectively acts like it's part of the vector table even though it's not technically
# Makes things nice and orthogonal for the assembly programmer (me)

################################################################################################################
# Vector table
################################################################################################################
.section .vector_table, "ax"
#.global __vector_table#NOT weak since this MUST go into this section
#__vector_table:
.global _start#NOT weak since this is the hardcoded entry point (it MUST go into this section)
_start:
    j   __crt0                                                  #Address: 0x00000000
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
__default_riscv_exception_and_interrupt_handler:#Called when a trap or interrupt occurs EXCEPT for reset by default

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
    .insn r CUSTOM_0, 0, 0, zero, zero, zero
