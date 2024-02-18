/**
 * @file    main.c
 * @brief   Entry point into the OGSBI (originally IRVE SBI/Firmware)
 * 
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  See the LICENSE file at the root of the project for licensing info.
 * 
 * TODO longer description
 *
*/

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include "rvsw.h"

#include "asm_c_interface.h"
#include "common.h"

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

int main(int, const char**) {
    uint32_t mhartid = RVSW_CSRR(mhartid);

    puts("OGSBI is starting up...");
    puts("  ___   ____ ____  ____ ___");
    puts(" / _ \\ / ___/ ___|| __ )_ _|");
    puts("| | | | |  _\\___ \\|  _ \\| |");
    puts("| |_| | |_| |___) | |_) | |");
    puts(" \\___/ \\____|____/|____/___|");
    puts("");
    puts("RVSW's First SBI Implementation");
    puts("Copyright (C) 2023-2024 John Jekel");
    puts("See the LICENSE file at the root of the project for licensing info.");
    puts("");
    puts("Configuration Info:");
    printf("  RISC-V Hart ID:                %ld\n",   mhartid);
    printf("  Device Tree Blob Address:      0x%lX\n", (uint32_t)&dtb_start);
    printf("  S-Mode / Kernel Entry Address: 0x%X\n",  RVSW_SMODE_AND_KERNEL_ENTRY_ADDR);
    puts("------------------------------------------------------------------------");
    puts("\n");

    //TODO print the reset cause (simply mcause) here

    dputs("Protecting OGSBI's memory...");
    dputs("TODO set PMP registers/etc");//TODO

    dputs("Delegating all interrupts and exceptions properly...");
    RVSW_CSRW(medeleg, 0b00000000000000001011000100000000);//TODO are we sure these are the ones we want to delegate? (user-mode ecall and page fault so far)
    RVSW_CSRW(mideleg, 0b00000000000000000000001000100010);//All S-Mode interrupts -> S-Mode, and all M-Mode interrupts -> M-Mode

    dputs("Clearing mip and enabling interrupts...");
    //OGSBI may implement some interrupts itself or may pass them along to S-Mode
    //Since hardware only ever triggers M-Mode interrupts, we may need to intersept them and set the s-mode analogs ourselves
    RVSW_CSR_ZERO(mip);//Clear all pending interrupts
    RVSW_CSRW(mie, (1 << 11) | (1 << 7) | (1 << 3));//Enable machine software, timer, and external interrupts
    RVSW_CSRW(mstatus, 1 << 3);//To enable global machine (NOT s-mode) interrupts

    //Set stvec to point to just after the kernel entry point
    //This is mostly to help out "nice" linker scripts that put the vector table right after the reset section
    RVSW_CSRW(stvec, (RVSW_SMODE_AND_KERNEL_ENTRY_ADDR + 4) | 0b01);//Just after the kernel entry point, vectored

    puts("Jumping to the kernel, cya later! :)");
    jump2linux(mhartid, (uint32_t)&dtb_start, RVSW_SMODE_AND_KERNEL_ENTRY_ADDR);//Never returns

    assert(false && "We should never get here!");
    exit(1);
}
