/* main.c
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Entry point into the IRVE SBI/Firmware
 *
*/

/* Constants And Defines */

//const static st:
#define KERNEL_ADDR 0xC0000000
#define DTB_ADDR 0xDEADBEEF//TODO

/* Includes */

#include <stdio.h>
#include <stdint.h>
#include "irve.h"

/* Types */

typedef struct {
    long error;//a0
    long value;//a1
} sbiret;

/* Variables */

//TODO

/* External Function Declarations */

//void jump2smode(uint32_t entry_addr);
void jump2linux(uint32_t hard_id, uint32_t dtb_addr, uint32_t kernel_addr);

/* Static Function Declarations */

//TODO

/* Function Implementations */

int main() {
#ifndef NDEBUG
    puts("IRVE SBI and Firmware starting...");
#endif

    __asm__ volatile("csrrw zero, medeleg, zero");//All exceptions are handled in M-Mode

    //TODO do initialization stuff here

    //jump2smode(SMODE_ENTRY_ADDR);
    jump2linux(0, DTB_ADDR, KERNEL_ADDR);//TODO instead of just assuming the hart id is 0, actually pass the contents of mhartid

    return 0;
}

sbiret handle_smode_ecall(uint32_t a0 __attribute__((unused)), uint32_t a1 __attribute__((unused)), uint32_t a2 __attribute__((unused)), uint32_t a3 __attribute__((unused)), uint32_t a4 __attribute__((unused)), uint32_t a5 __attribute__((unused)), uint32_t FID __attribute__((unused)), uint32_t EID __attribute__((unused))) {
    puts("TEMP TESTING");//TESTING
    irve_exit();//TODO actually implement SBI calls
}

//__attribute__ ((interrupt ("machine"))) void __riscv_synchronous_exception_and_user_mode_swi_handler(void) {
     
//}

/* Static Function Implementations */

//TODO
