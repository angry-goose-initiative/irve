/* main.c
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Entry point into the IRVE SBI/Firmware
 *
*/

/* Constants And Defines */

//const static st:
#define HART_ID 0//TODO instead of just assuming the hart id is 0, actually pass the contents of mhartid
#define KERNEL_ADDR 0xC0000000
#define DTB_ADDR 0xDEADBEEF//TODO

/* Includes */

#include "asm_c_interface.h"

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include "irve.h"

/* Macros */

#ifndef NDEBUG
#define dputs(str) puts(str)
#define dprintf(...) printf(__VA_ARGS__)
#else
#define dputs(str) do {} while (0)
#define dprintf(...) do {} while (0)
#endif

/* Static Function Declarations */

//TODO

/* Function Implementations */

int main() {
    dputs("IRVE SBI and Firmware starting...");
    dputs("Copyright (C) 2023 John Jekel and Nick Chan");

    dputs("Configuration Info:");
    dprintf("  HART_ID:     %d\n",   HART_ID);
    dprintf("  DTB_ADDR:    0x%X\n", DTB_ADDR);
    dprintf("  KERNEL_ADDR: 0x%X\n", KERNEL_ADDR);

    dputs("Delegating all interrupts and exceptions to M-Mode...");
    __asm__ volatile("csrrw zero, medeleg, zero");//All exceptions are handled in M-Mode
    __asm__ volatile("csrrw zero, mideleg, zero");//All interrupts are handled in M-Mode

    //TODO do other initialization stuff here

    dputs("Jumping to Linux...");
    jump2linux(HART_ID, DTB_ADDR, KERNEL_ADDR);//Never returns

    assert(false && "We should never get here!");
}

sbiret_t handle_smode_ecall(
    uint32_t a0  __attribute__((unused)), 
    uint32_t a1  __attribute__((unused)),
    uint32_t a2  __attribute__((unused)),
    uint32_t a3  __attribute__((unused)),
    uint32_t a4  __attribute__((unused)),
    uint32_t a5  __attribute__((unused)),
    uint32_t FID __attribute__((unused)),
    uint32_t EID __attribute__((unused))
) {
    dputs("Recieved S-Mode ECALL");
    dprintf("  a0:  0x%lX\n", a0);
    dprintf("  a1:  0x%lX\n", a1);
    dprintf("  a2:  0x%lX\n", a2);
    dprintf("  a3:  0x%lX\n", a3);
    dprintf("  a4:  0x%lX\n", a4);
    dprintf("  a5:  0x%lX\n", a5);
    dprintf("  FID: 0x%lX\n", FID);
    dprintf("  EID: 0x%lX\n", EID);

    sbiret_t result;
    switch (EID) {
        case 0x10:
            dputs("Base Extension");
            switch (FID) {
                case 0:
                    dputs("Function: sbi_get_spec_version()\n");
                    result.error = SBI_SUCCESS;
                    result.value = 0b00000001000000000000000000000000;//We implement Version 1.0.0 (the latest)
                    break;
                //TODO others
                default:
                    dputs("Invalid or unsupported Base Extension function!");
                    result.error = SBI_ERR_NOT_SUPPORTED;
                    break;
            }
            break;
        default:
            dputs("Invalid or unsupported SBI extension!");
            result.error = SBI_ERR_NOT_SUPPORTED;
            break;
    }

    return result;
}

void handle_other_exceptions(/* TODO decide args, if we actualy will be doing this in C at all */) {
    assert(false && "TODO this function will need to be done in assembly as it needs fine-grained control of registers and the stack");
}

/* Static Function Implementations */

//TODO
