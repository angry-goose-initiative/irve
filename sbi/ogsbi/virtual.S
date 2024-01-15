# virtual.S
# Copyright (C) 2023-2024 John Jekel
# See the LICENSE file at the root of the project for licensing info.
#
# Code for reading from virtual memory from M-Mode.
# ASSUMES INTERRUPTS AREN'T ENABLED, OTHERWISE THIS COULD BREAK OTHER THINGS BECAUSE IT MAY SET mstatus.MPRV
# ALSO ASSUMES WE ARE IN AN EXCEPTION (so mstatus.MPP is set correctly)
#

#TODO handle the case where the virtual memory pointed to is invalid or not readable (likely handle this in the exception handler)

.macro create_virtual_fn name, instr:vararg
.global virtual_\name
virtual_\name:#a0 is a pointer to a VIRTUAL address (or, if S-mode was in Bare, a physical address). a1 is the value to write (if this is a write function)
    #Preserve the original mstatus in t0
    csrr t0, mstatus

    #Set mstatus.MPRV to 1
    li t1, (1 << 17)
    or t1, t0, t1
    csrw mstatus, t1

    #Now do the instruction
    \instr

    #Finally set mstatus.MPRV back to 0
    csrw mstatus, t0

    ret
.endm

create_virtual_fn read_byte,            lbu a0, 0(a0)
create_virtual_fn read_halfword,        lhu a0, 0(a0)
create_virtual_fn read_word,            lw a0,  0(a0)
create_virtual_fn read_signed_byte,     lb a0,  0(a0)
create_virtual_fn read_signed_halfword, lh a0,  0(a0)

create_virtual_fn write_byte,           sb a1,  0(a0)
create_virtual_fn write_halfword,       sh a1,  0(a0)
create_virtual_fn write_word,           sw a1,  0(a0)
