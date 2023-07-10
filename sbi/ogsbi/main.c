/**
 * @file    main.c
 * @brief   Entry point into the OGSBI (originally IRVE SBI/Firmware)
 * 
 * @copyright Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 * 
 * TODO longer description
 *
*/

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include "rvsw_asm.h"
#include "asm_c_interface.h"
#include "common.h"

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/* ------------------------------------------------------------------------------------------------
 * Static Function Declarations
 * --------------------------------------------------------------------------------------------- */

uint32_t mhartid(void);

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

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
    dprintf("  RISC-V Hart ID:                %ld",   mhartid());
    dprintf("  Device Tree Blob Address:      0x%lX", (uint32_t)&dtb_start);
    dprintf("  S-Mode / Kernel Entry Address: 0x%X",  RVSW_SMODE_AND_KERNEL_ENTRY_ADDR);
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
        : [ADDR] "r" (RVSW_SMODE_AND_KERNEL_ENTRY_ADDR + 4)//Just after the kernel entry point
        : /* No clobbered registers */
    );

    dputs("Jumping to the kernel, cya later!");
    jump2linux(mhartid(), (uint32_t)&dtb_start, RVSW_SMODE_AND_KERNEL_ENTRY_ADDR);//Never returns

    assert(false && "We should never get here!");
    exit(1);
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
