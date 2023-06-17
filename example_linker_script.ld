/* irve_mmode.ld
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Linker script for irve RISC-V M-Mode software
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

/* The arch is rv32 */
OUTPUT_ARCH("riscv")
OUTPUT_FORMAT("elf32-littleriscv", "elf32-littleriscv", "elf32-littleriscv")

ENTRY(___rvsw_reset___)

SECTIONS
{
    PROVIDE(___rvsw_stack_top___ = ORIGIN(ram) + LENGTH(ram));
    
    .init : { *(.init*) } > ram
    .vector_table : { *(.vector_table*) } > ram
    .text : { *(.text*) } > ram
    .rodata : { *(.rodata*) } > ram
    .sdata : { *(.sdata*) } > ram
    .data : { *(.data*) } > ram
    .bss : {
        ___rvsw_bss_start___ = .;
        *(.bss*)
        ___rvsw_bss_end___ = .;
    } > ram

    /* TODO sections for init and fini array: https://blog.thea.codes/the-most-thoroughly-commented-linker-script/ */

    .heap : {
        end = .;/* We do this to tell Newlib's default sbrk implementation that here is the end of globals/ the beginning of the heap; see https://github.com/eblot/newlib/blob/master/libgloss/sbrk.c */
        *(.heap*)
    } > ram

    ___rvsw_global_pointer___$ = .; /* The global pointer points to the beginning of the SDATA section */
}
