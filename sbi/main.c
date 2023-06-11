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

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include "irve.h"

/* Types */

typedef struct {
    long error;//a0
    long value;//a1
} sbiret;

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

sbiret handle_smode_ecall(
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
    dprintf("  FID: 0x%lx\n", FID);
    dprintf("  EID: 0x%lx\n", EID);

    assert(false && "TODO implement");//TODO actually implement SBI calls
}

void handle_other_exceptions(/* TODO decide args, if we actualy will be doing this in C at all */) {
    assert(false && "TODO this function will need to be done in assembly as it needs fine-grained control of registers and the stack");
}

/* Static Function Implementations */

//TODO
