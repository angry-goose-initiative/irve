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

//SBI Constants
#define SBI_SUCCESS                 0
#define SBI_ERR_FAILED              -1
#define SBI_ERR_NOT_SUPPORTED       -2
#define SBI_ERR_INVALID_PARAM       -3
#define SBI_ERR_DENIED              -4
#define SBI_ERR_INVALID_ADDRESS     -5
#define SBI_ERR_ALREADY_AVAILABLE   -6
#define SBI_ERR_ALREADY_STARTED     -7
#define SBI_ERR_ALREADY_STOPPED     -8
#define SBI_ERR_INVALID_HART_ID     -9

/* Includes */

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include "irve.h"

/* Types */

typedef struct {
    long error;//a0
    long value;//a1
} sbiret_t;

/* Macros */

#ifndef NDEBUG
#define dputs(str) puts(str)
#define dprintf(...) printf(__VA_ARGS__)
#else
#define dputs(str) do {} while (0)
#define dprintf(...) do {} while (0)
#endif

/* Variables */

//Needs to be preserved so, when returning from S-Mode, we can still use C if the compiler depends on these
uint32_t mmode_preserved_gp;
uint32_t mmode_preserved_tp;

/* External Function Declarations */

void jump2linux(uint32_t hart_id, uint32_t dtb_addr, uint32_t kernel_addr);

/* Static Function Declarations */

//TODO

/* Function Implementations */

int main() {
    dputs("IRVE SBI and Firmware starting...");
    dputs("Copyright (C) 2023 John Jekel and Nick Chan");

    dputs("Configuration Info:");
    dprintf("  HART_ID:     %d\n",   HART_ID);
    dprintf("  DTB_ADDR:    0x%x\n", DTB_ADDR);
    dprintf("  KERNEL_ADDR: 0x%x\n", KERNEL_ADDR);

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
    dprintf("  a0:  0x%lx\n", a0);
    dprintf("  a1:  0x%lx\n", a1);
    dprintf("  a2:  0x%lx\n", a2);
    dprintf("  a3:  0x%lx\n", a3);
    dprintf("  a4:  0x%lx\n", a4);
    dprintf("  a5:  0x%lx\n", a5);
    dprintf("  FID: 0x%lx\n", FID);
    dprintf("  EID: 0x%lx\n", EID);

    sbiret_t result;
    switch (EID) {
        case 0x10:
            dputs("Base Extension");
            assert(false && "TODO implement");//TODO
            break;
        default:
            dputs("Invalid or unsupported SBI call!");
            result.error = SBI_ERR_NOT_SUPPORTED;
    }

    return result;
}

void handle_other_exceptions(/* TODO decide args, if we actualy will be doing this in C at all */) {
    assert(false && "TODO this function will need to be done in assembly as it needs fine-grained control of registers and the stack");
}

/* Static Function Implementations */

//TODO
