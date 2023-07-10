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

#include <stdio.h>

/* ------------------------------------------------------------------------------------------------
 * Constants/Defines
 * --------------------------------------------------------------------------------------------- */

#define HART_ID 0 //TODO
#define DTB_ADDR 0x00001001 //TODO (misaligned so it is ignored, add logic to provide a proper address)
#define KERNEL_ADDR 0xC0000000

/* ------------------------------------------------------------------------------------------------
 * Type Declarations
 * --------------------------------------------------------------------------------------------- */

//TODO

/* ------------------------------------------------------------------------------------------------
 * Static Variables
 * --------------------------------------------------------------------------------------------- */

//TODO

/* ------------------------------------------------------------------------------------------------
 * Static Function Declarations
 * --------------------------------------------------------------------------------------------- */

//TODO

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
    puts("Copyright (C) 2023 John Jekel and Nick Chan");
    puts("See the LICENSE file at the root of the project for licensing info.");
    putc('\n', stdout);
    puts("Configuration Info:");
    printf("  HART_ID:     %d\n",   HART_ID);
    printf("  DTB_ADDR:    0x%X\n", DTB_ADDR);
    printf("  KERNEL_ADDR: 0x%X\n", KERNEL_ADDR);
    puts("------------------------------------------------------------------------");
    putc('\n', stdout);

    puts("Alrighty, well off to the kernel with you then!");
    __asm__ volatile (
        "mv a0, %[hart_id]\n"
        "mv a1, %[dtb_addr]\n"
        "mv t0, %[kernel_addr]\n"
        "jr t0\n"//Cya!
        : /* No output registers */
        : [hart_id] "r" (HART_ID), [dtb_addr] "r" (DTB_ADDR), [kernel_addr] "r" (KERNEL_ADDR)
        : "a0", "a1", "t0"//Not that it matters since we never return
    );

    return 0;
}

/* ------------------------------------------------------------------------------------------------
 * Static Function Implementations
 * --------------------------------------------------------------------------------------------- */

//TODO
