/**
 * @file    other_exceptions.c
 * @brief   Code to handle non-S-Mode ECALL exceptions
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

void handle_other_exceptions(uint32_t registers[31], uint32_t mcause, uint32_t mepc) {
    dputs("Recieved a non-S-Mode-ECALL exception");
    for (int i = 0; i < 31; ++i) {
        dprintf("  x%d:\t0x%lX", i + 1, registers[i]);
    }
    dprintf("  mcause: 0x%lX", mcause);
    dprintf("  mepc:   0x%lX", mepc);

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
            assert(false && "TODO implement (emulate some instructions, and delegate others to S-Mode)");
            break;
        case 3://BREAKPOINT_EXCEPTION
            dputs("Breakpoint Exception");
            dputs("  OGSBI does not support debugging S-Mode code.");
            dputs("  I give up!");
            exit(1);
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
            assert(false && "We should never get here; OGSBI SBI never does ECALL!");
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
