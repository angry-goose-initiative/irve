/* main.c
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Entry point into the OGSBI (originally IRVE SBI/Firmware)
 *
*/

/* Constants And Defines */

#define HART_ID 0//TODO instead of just assuming the hart id is 0, actually pass the contents of mhartid
//#define KERNEL_ADDR 0xC0000000
#define KERNEL_ADDR 0x02000000//TEMPORARY until memory-things are ready
#define DTB_ADDR 0xDEADBEEF//TODO

/* Includes */

#include "asm_c_interface.h"
#include "common.h"

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/* Static Function Declarations */

//TODO

/* Function Implementations */

int main(int, const char**) {
    dputs("OGSBI is starting up...");
    dputs("  ___   ____ ____  ____ ___");
    dputs(" / _ \\ / ___/ ___|| __ )_ _|");
    dputs("| | | | |  _\\___ \\|  _ \\| |");
    dputs("| |_| | |_| |___) | |_) | |");
    dputs(" \\___/ \\____|____/|____/___|");
    dputc('\n');
    dputs("RVSW's First SBI Implementation");
    dputs("Copyright (C) 2023 John Jekel and Nick Chan");
    dputs("See the LICENSE file at the root of the project for licensing info.");
    dputc('\n');
    dputs("Configuration Info:");
    dprintf("  HART_ID:     %d",   HART_ID);
    dprintf("  DTB_ADDR:    0x%X", DTB_ADDR);
    dprintf("  KERNEL_ADDR: 0x%X", KERNEL_ADDR);
    dputs("------------------------------------------------------------------------");
    dputc('\n');
    dputc('\n');

    //TODO print the reset cause (simply mcause) here

    dputs("Protecting OGSBI's memory...");
    dputs("TODO");//TODO

    dputs("Delegating all interrupts and exceptions properly...");
    __asm__ volatile (//sbi_debug_console_write()
        "li t0, 0b00000000000000001011000100000000\n"//TODO are we sure these are the ones we want to delegate? (user-mode ecall and page fault so far)
        //"li t0, 0b00000000000000000000000000000000\n"//TESTING for some initial debugging / bringup stuff
        "csrw medeleg, t0\n"
        "li t0, 0b00000000000000000000001000100010\n"//All S-Mode interrupts -> S-Mode, and all M-Mode interrupts -> M-Mode
        "csrw mideleg, t0\n"
        : /* No output registers */
        : /* No input registers */
        : "t0", "t1"
    );

    dputs("Clearing mip and enabling interrupts...");
    dputs("TODO");//TODO

    //TODO do other initialization stuff here

    //Set stvec to point to just after the kernel entry point
    //This is mostly to help out "nice" linker scripts that put the vector table right after the reset section
    __asm__ volatile (
        "csrw stvec, %[ADDR]\n"
        : /* No output registers */
        : [ADDR] "r" (KERNEL_ADDR + 4)//Just after the kernel entry point
        : /* No clobbered registers */
    );

    dputs("Jumping to the kernel, cya later!");
    jump2linux(HART_ID, DTB_ADDR, KERNEL_ADDR);//Never returns

    assert(false && "We should never get here!");
    exit(1);
}


/* Static Function Implementations */

//TODO
