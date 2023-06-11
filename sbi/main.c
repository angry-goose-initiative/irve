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
#define dputs(str) puts("IRVESBI> " str)
#define dprintf(...) printf("IRVESBI> " __VA_ARGS__)
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

    dputs("Jumping to the kernel, cya later!");
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

void handle_other_exceptions(uint32_t registers[31], uint32_t mcause, uint32_t mepc) {
    dputs("Recieved a non-S-Mode-ECALL exception");
    for (int i = 1; i < 32; i++) {
        dprintf("  x%d:\t0x%lX\n", i, registers[i - 1]);
    }
    dprintf("  mcause: 0x%lX\n", mcause);
    dprintf("  mepc:   0x%lX\n", mepc);

    switch (mcause) {
        case 0://INSTRUCTION_ADDRESS_MISALIGNED_EXCEPTION
            dputs("Instruction Address Misaligned Exception");
            assert(false && "TODO implement misaligned accesses");
            break;
        case 1://INSTRUCTION_ACCESS_FAULT_EXCEPTION
            dputs("Instruction Access Fault Exception");
            dputs("  S-Mode code has just attempted to fetch an instruction from invalid memory or memory protected by PMP");
            dputs("  This is likely a bug in S-Mode code!");
            dputs("  I give up!");
            irve_exit();
            break;
        case 2://ILLEGAL_INSTRUCTION_EXCEPTION
            dputs("Illegal Instruction Exception");
            assert(false && "TODO implement (emulate some instructions)");
            break;
        case 3://BREAKPOINT_EXCEPTION
            dputs("Breakpoint Exception");
            assert(false && "TODO implement (what to do for this?)");
            break;
        case 4://LOAD_ADDRESS_MISALIGNED_EXCEPTION
            dputs("Load Address Misaligned Exception");
            assert(false && "TODO implement misaligned accesses");
            break;
        case 5://LOAD_ACCESS_FAULT_EXCEPTION
            dputs("Load Access Fault Exception");
            dputs("  S-Mode code has just attempted to read from invalid memory or memory protected by PMP");
            dputs("  This is likely a bug in S-Mode code!");
            dputs("  I give up!");
            irve_exit();
            break;
        case 6://STORE_OR_AMO_ADDRESS_MISALIGNED_EXCEPTION
            dputs("Store or AMO Address Misaligned Exception");
            assert(false && "TODO implement misaligned accesses (don't even bother with AMOs, just stores)");
            break;
        case 7://STORE_OR_AMO_ACCESS_FAULT_EXCEPTION
            dputs("Store or AMO Access Fault Exception");
            dputs("  S-Mode code has just attempted to store to or perform an AMO on invalid memory or memory protected by PMP");
            dputs("  This is likely a bug in S-Mode code!");
            dputs("  I give up!");
            irve_exit();
            break;
        case 8://UMODE_ECALL_EXCEPTION
            assert(false && "We should never get here; UMODE_ECALL_EXCEPTIONs should be delegated to S-Mode");
            irve_exit();
            break;
        case 9://SMODE_ECALL_EXCEPTION
            assert(false && "We should never get here; this should be handled by handle_smode_ecall()");
            irve_exit();
            break;
        case 11://MMODE_ECALL_EXCEPTION
            assert(false && "We should never get here; The IRVE SBI never does ECALL!");
            irve_exit();
            break;
        case 12://INSTRUCTION_PAGE_FAULT_EXCEPTION
            dputs("Instruction Page Fault Exception");
            dputs("  S-Mode code has just attempted to fetch and instruction from invalid virtual memory");
            dputs("  This is likely a bug in S-Mode code!");
            dputs("  I give up!");
            irve_exit();
            break;
        case 13://LOAD_PAGE_FAULT_EXCEPTION
            dputs("Load Page Fault Exception");
            dputs("  S-Mode code has just attempted read from invalid virtual memory");
            dputs("  This is likely a bug in S-Mode code!");
            dputs("  I give up!");
            irve_exit();
            break;
        case 15://STORE_OR_AMO_PAGE_FAULT_EXCEPTION
            dputs("Store or AMO Page Fault Exception");
            dputs("  S-Mode code has just attempted to store to or perform an AMO on invalid virtual memory");
            dputs("  This is likely a bug in S-Mode code!");
            dputs("  I give up!");
            irve_exit();
            break;
    }
}

/* Static Function Implementations */

//TODO
