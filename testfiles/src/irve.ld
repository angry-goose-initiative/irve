/* irve.ld
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Linker script for irve RISC-V c software
 *
 * Based on code from jzjcoresoftware, which was in turn based on code from the FSF:
 *
 * Copyright (C) 2014-2020 Free Software Foundation, Inc.
 * Copying and distribution of this script, with or without modification,
 * are permitted in any medium without royalty provided the copyright
 * notice and this notice are preserved.
*/

MEMORY
{
    /* Default memory size is 64M and starts at 0x00000000 */
    ram : ORIGIN = 0x00000000, LENGTH = 64M
}

/* the arch is rv32 */
OUTPUT_ARCH("riscv")
OUTPUT_FORMAT("elf32-littleriscv", "elf32-littleriscv", "elf32-littleriscv")

/* _start is defined in crt0.s */
ENTRY(_start)

SECTIONS
{
    PROVIDE (__executable_start = ORIGIN(ram));
    PROVIDE(__stack_top = ORIGIN(ram) + LENGTH(ram));
    
    .vector_table : { *(.vector_table*) } > ram
    .init : { *(.init*) } > ram
    .text : { *(.text*) } > ram
    .rodata : { *(.rodata*) } > ram
    .sdata :
    {
        __SDATA_BEGIN__ = .; 
        *(.sdata*)
    } > ram
    .data : { *(.data*) } > ram
    .bss : { *(.bss*) } > ram

    /* TODO I basically guessed this, is this correct? */
    .heap : { 
        __heap_start__ = .;
        end = __heap_start__;
        *(.heap*)
    } > ram

    __global_pointer$ = .; /* The global pointer points to the beginning of the SDATA section */
}
