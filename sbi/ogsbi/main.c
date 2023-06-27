/* main.c
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Entry point into the OGSBI (originally IRVE SBI/Firmware)
 *
*/

/* Constants And Defines */

//const static st:
#define HART_ID 0//TODO instead of just assuming the hart id is 0, actually pass the contents of mhartid
//#define KERNEL_ADDR 0xC0000000
#define KERNEL_ADDR 0x02000000//TEMPORARY until memory-things are ready
#define DTB_ADDR 0xDEADBEEF//TODO

/* Includes */

#include "asm_c_interface.h"

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/* Macros */

#ifndef NDEBUG
#define dputs(str) puts("OGSBI> " str)
#define dprintf(...) printf("OGSBI> " __VA_ARGS__)
#else
#define dputs(str) do {} while (0)
#define dprintf(...) do {} while (0)
#endif

/* Static Function Declarations */

//TODO

/* Function Implementations */

int main() {
    dputs("OGSBI starting...");
    dputs("Copyright (C) 2023 John Jekel and Nick Chan");

    dputs("Configuration Info:");
    dprintf("  HART_ID:     %d\n",   HART_ID);
    dprintf("  DTB_ADDR:    0x%X\n", DTB_ADDR);
    dprintf("  KERNEL_ADDR: 0x%X\n", KERNEL_ADDR);

    //TODO print the reset cause (simply mcause) here

    dputs("Delegating all interrupts and exceptions properly...");
    __asm__ volatile (//sbi_debug_console_write()
        "li t0, 0b00000000000000001011000100000000\n"//TODO are we sure these are the ones we want to delegate? (user-mode ecall and page fault so far)
        "csrw medeleg, t0\n"
        "li t0, 0b00000000000000000000001000100010\n"//All S-Mode interrupts -> S-Mode, and all M-Mode interrupts -> M-Mode
        "csrw mideleg, t0\n"
        : /* No output registers */
        : /* No input registers */
        : "t0", "t1"
    );

    //TODO do other initialization stuff here

    dputs("Jumping to the kernel, cya later!");
    jump2linux(HART_ID, DTB_ADDR, KERNEL_ADDR);//Never returns

    assert(false && "We should never get here!");
}

sbiret_t handle_sbi_smode_ecall(
    uint32_t a0  __attribute__((unused)), 
    uint32_t a1  __attribute__((unused)),
    uint32_t a2  __attribute__((unused)),
    uint32_t a3  __attribute__((unused)),
    uint32_t a4  __attribute__((unused)),
    uint32_t a5  __attribute__((unused)),
    uint32_t FID __attribute__((unused)),
    uint32_t EID
) {
    dputs("Recieved S-Mode ECALL (SBI Call)");
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
        case 0x00 ... 0x0F:
            assert(false && "We should never get here (the legacy handler should have been called instead)!");
            exit(1);
            break;
        case 0x10:
            dputs("Base Extension");
            switch (FID) {
                case 0:
                    dputs("Function: sbi_get_spec_version()");
                    result.error = SBI_SUCCESS;
                    result.value = 0b00000001000000000000000000000000;//We implement Version 1.0.0 (the latest)
                    dprintf("  SBI Spec Version: 0x%lX\n", result.value);
                    break;
                case 1:
                    dputs("Function: sbi_get_impl_id()");
                    result.error = SBI_SUCCESS;
                    result.value = 0xABCD1234;//TODO we'd likely need to ask the foundation for an ID
                    dprintf("  Implementation ID: 0x%lX", result.value);
                    break;
                case 2:
                    dputs("Function: sbi_get_impl_version()");
                    result.error = SBI_SUCCESS;
                    result.value = 0;//TODO decide how we'll encode version information
                    dprintf("  Implementation Version: 0x%lX", result.value);
                    break;
                case 3:
                    dputs("Function: sbi_probe_extension()");
                    result.error = SBI_SUCCESS;
                    switch (a0) {
                        case 0x08:          result.value = 1; break;//LEGACY Function: sbi_shutdown()
                        case 0x10:          result.value = 1; break;//Base Extension
                        case 0x4442434E:    result.value = 1; break;//Debug Console Extension
                        default:            result.value = 0; break;//Unsupported or non-existent extension
                    }
                    dprintf("  Extension 0x%lX is %s\n", a0, result.value ? "supported" : "unsupported");
                    break;
                case 4:
                    dputs("Function: sbi_get_mvendorid()");
                    result.error = SBI_SUCCESS;
                    __asm__ volatile (
                        "csrr %[rd], mvendorid"
                        : [rd] "=r" (result.value)
                        : /* No source registers */
                        : /* No clobbered registers */
                    );
                    dprintf("  mvendorid: 0x%lX\n", result.value);
                    break;
                case 5:
                    dputs("Function: sbi_get_marchid()");
                    result.error = SBI_SUCCESS;
                    __asm__ volatile (
                        "csrr %[rd], marchid"
                        : [rd] "=r" (result.value)
                        : /* No source registers */
                        : /* No clobbered registers */
                    );
                    dprintf("  marchid: 0x%lX\n", result.value);
                    break;
                case 6:
                    dputs("Function: sbi_get_mimpid()");
                    result.error = SBI_SUCCESS;
                    __asm__ volatile (
                        "csrr %[rd], mimpid"
                        : [rd] "=r" (result.value)
                        : /* No source registers */
                        : /* No clobbered registers */
                    );
                    dprintf("  mimpid: 0x%lX\n", result.value);
                    break;
                default:
                    dputs("Invalid or unsupported Base Extension function!");
                    result.error = SBI_ERR_NOT_SUPPORTED;
                    break;
            }
            break;
        case 0x4442434E:
            dputs("Debug Console Extension");
            switch (FID) {
                case 0:
                    dputs("Function: sbi_debug_console_write()");
                    //TODO handle the case where base_addr is not valid (including when base_addr_hi is any non-zero value)
                    for (size_t i = 0; i < a0; ++i) {
                        putc(((char*)a1)[i], stdout);
                    }
                    result.error = SBI_SUCCESS;
                    result.value = a0;
                    break;
                case 1:
                    dputs("Function: sbi_debug_console_read()");
                    //TODO handle the case where base_addr is not valid (including when base_addr_hi is any non-zero value)
                    for (size_t i = 0; i < a0; ++i) {
                        ((char*)a1)[i] = getc(stdin);
                    }
                    result.error = SBI_SUCCESS;
                    result.value = a0;
                    break;
                case 2:
                    dputs("Function: sbi_debug_console_write_byte()");
                    //TODO handle the case where base_addr is not valid (including when base_addr_hi is any non-zero value)
                    putc((char)a0, stdout);
                    result.error = SBI_SUCCESS;
                    result.value = 0;
                    break;
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

long handle_legacy_sbi_smode_ecall(
    uint32_t a0  __attribute__((unused)), 
    uint32_t a1  __attribute__((unused)),
    uint32_t a2  __attribute__((unused)),
    uint32_t a3  __attribute__((unused)),
    uint32_t a4  __attribute__((unused)),
    uint32_t a5  __attribute__((unused)),
    uint32_t a6  __attribute__((unused)),
    uint32_t EID
) {
    dputs("Recieved S-Mode ECALL (LEGACY SBI Call)");
    dprintf("  a0:  0x%lX\n", a0);
    dprintf("  a1:  0x%lX\n", a1);
    dprintf("  a2:  0x%lX\n", a2);
    dprintf("  a3:  0x%lX\n", a3);
    dprintf("  a4:  0x%lX\n", a4);
    dprintf("  a5:  0x%lX\n", a5);
    dprintf("  a6:  0x%lX\n", a6);
    dprintf("  EID: 0x%lX\n", EID);

    long result;
    switch (EID) {
        case 0x08:
            dputs("LEGACY Function: sbi_shutdown()");
            dputs("  Shutting down, au revoir! ...");
            exit(0);
            break;
        //TODO others
        default:
            assert(false && "We should never get here (the new handler should have been called instead)!");
            break;
    }

    return result;
}

void handle_other_exceptions(uint32_t registers[31], uint32_t mcause, uint32_t mepc) {
    dputs("Recieved a non-S-Mode-ECALL exception");
    for (int i = 0; i < 31; ++i) {
        dprintf("  x%d:\t0x%lX\n", i + 1, registers[i]);
    }
    dprintf("  mcause: 0x%lX\n", mcause);
    dprintf("  mepc:   0x%lX\n", mepc);

    //TODO if the kernel is paging part of itself out, it may need the ex. page faults delegated to it, not use UMode ecalls
    //Similarly for access faults potentially...
    //We may need to delegate more than we think to S-Mode

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
            exit(1);
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
            exit(1);
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
            exit(1);
            break;
        case 8://UMODE_ECALL_EXCEPTION
            assert(false && "We should never get here; UMODE_ECALL_EXCEPTIONs should be delegated to S-Mode");
            exit(1);
            break;
        case 9://SMODE_ECALL_EXCEPTION
            assert(false && "We should never get here; this should be handled by handle_smode_ecall()");
            exit(1);
            break;
        case 11://MMODE_ECALL_EXCEPTION
            assert(false && "We should never get here; The IRVE SBI never does ECALL!");
            exit(1);
            break;
        case 12://INSTRUCTION_PAGE_FAULT_EXCEPTION
            dputs("Instruction Page Fault Exception");
            dputs("  S-Mode code has just attempted to fetch and instruction from invalid virtual memory");
            dputs("  This is likely a bug in S-Mode code!");
            dputs("  I give up!");
            exit(1);
            break;
        case 13://LOAD_PAGE_FAULT_EXCEPTION
            dputs("Load Page Fault Exception");
            dputs("  S-Mode code has just attempted read from invalid virtual memory");
            dputs("  This is likely a bug in S-Mode code!");
            dputs("  I give up!");
            exit(1);
            break;
        case 15://STORE_OR_AMO_PAGE_FAULT_EXCEPTION
            dputs("Store or AMO Page Fault Exception");
            dputs("  S-Mode code has just attempted to store to or perform an AMO on invalid virtual memory");
            dputs("  This is likely a bug in S-Mode code!");
            dputs("  I give up!");
            exit(1);
            break;
    }
}

/* Static Function Implementations */

//TODO
