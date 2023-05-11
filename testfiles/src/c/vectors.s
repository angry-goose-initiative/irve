# crt0.s
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

.section .vectors, "ax"
.global __vectors#NOT weak since this MUST go into this section
__vectors:
.global _start#NOT weak since this is the hardcoded entry point (it also MUST go into this section)
_start:
    j   __crt0#Address: 0x00000000
    j   __default_interrupt_and_trap_handler#Address: 0x00000004
    #TODO if this becomes vectored, this will need updating

.weak __default_interrupt_and_trap_handler#By default, if there is no interrupt handler, halt the cpu
__default_interrupt_and_trap_handler:#Called when a trap or interrupt occurs EXCEPT for reset
    .insn r CUSTOM_0, 0, 0, zero, zero, zero
