/**
 * @file    nommulinux.c
 * @brief   Jump to the a nommu (aka. M-mode) Linux kernel
 * 
 * @copyright Copyright (C) 2023 John Jekel
 * See the LICENSE file at the root of the project for licensing info.
 * 
 * TODO longer description
 *
*/

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include "rvsw_asm.h"

#include <stdio.h>

/* ------------------------------------------------------------------------------------------------
 * External Variables
 * --------------------------------------------------------------------------------------------- */

extern const uint8_t dtb_start;

/* ------------------------------------------------------------------------------------------------
 * Static Function Declarations
 * --------------------------------------------------------------------------------------------- */

uint32_t mhartid(void);

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

int main (int, const char**) {
    puts("A good day to you, friend!");
    puts("                                       _ _");
    puts(" _ __   ___  _ __ ___  _ __ ___  _   _| (_)_ __  _   ___  __");
    puts("| '_ \\ / _ \\| '_ ` _ \\| '_ ` _ \\| | | | | | '_ \\| | | \\ \\/ /");
    puts("| | | | (_) | | | | | | | | | | | |_| | | | | | | |_| |>  <");
    puts("|_| |_|\\___/|_| |_| |_|_| |_| |_|\\__,_|_|_|_| |_|\\__,_/_/\\_\\");
    putc('\n', stdout);
    puts("A simple nommu Linux bootloader for RISC-V");
    puts("Copyright (C) 2023 John Jekel");
    puts("See the LICENSE file at the root of the project for licensing info.");
    putc('\n', stdout);
    puts("Configuration Info:");
    printf("  RISC-V Hart ID:                    %ld\n",   mhartid());
    printf("  Device Tree Blob Address:          0x%lX\n", (uint32_t)&dtb_start);
    printf("  New M-Mode / Kernel Entry Address: 0x%X\n",  RVSW_SMODE_AND_KERNEL_ENTRY_ADDR);
    puts("------------------------------------------------------------------------");
    putc('\n', stdout);

    //Set mtvec to point to just after the kernel entry point
    //This is mostly to help out "nice" linker scripts that put the vector table right after the reset section
    __asm__ volatile (
        "csrw mtvec, %[ADDR]\n"
        : /* No output registers */
        : [ADDR] "r" (RVSW_SMODE_AND_KERNEL_ENTRY_ADDR + 4)//Just after the kernel entry point
        : /* No clobbered registers */
    );

    puts("Alrighty, well off to the kernel with you then!");
    __asm__ volatile (
        "mv a0, %[hart_id]\n"
        "mv a1, %[dtb_addr]\n"
        "mv t0, %[kernel_addr]\n"
        "jr t0\n"//Cya!
        : /* No output registers */
        : [hart_id] "r" (mhartid()), [dtb_addr] "r" ((uint32_t)&dtb_start), [kernel_addr] "r" (RVSW_SMODE_AND_KERNEL_ENTRY_ADDR)
        : "a0", "a1", "t0"//Not that it matters since we never return
    );

    return 0;
}

/* ------------------------------------------------------------------------------------------------
 * Static Function Implementations
 * --------------------------------------------------------------------------------------------- */

uint32_t mhartid(void) {
    uint32_t value;
    __asm__ volatile (
        "csrr %[value], mhartid\n"
        : [value] "=r" (value)
        : /* No inputs */
        : /* No clobbered registers */
    );
    return value;
}

/* ------------------------------------------------------------------------------------------------
 * Some end-of-file weirdness
 * --------------------------------------------------------------------------------------------- */

//NOTE: Including the dtb in this way must occur at the end of the file otherwise it would break all
//code below it
__asm__ (
    ".section .rodata\n"
    ".align 3\n"//THIS IS VERY IMPORTANT (must align to 8 bytes, or 2^3)
    "dtb_start:\n"
    ".incbin \"" RVSW_DTB_PATH "\""
);
