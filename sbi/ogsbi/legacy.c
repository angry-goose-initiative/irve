/**
 * @file    legacy.c
 * @brief   Code to handle legacy SBI calls
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

//TODO remove the __attribute__((unused))s
long handle_legacy_sbi_smode_ecall(
    uint32_t a0,
    uint32_t a1 __attribute__((unused)),
    uint32_t a2 __attribute__((unused)),
    uint32_t a3 __attribute__((unused)),
    //No legacy SBI calls use a4 thru a6
    uint32_t /* a4 */,
    uint32_t /* a5 */,
    uint32_t /* a6 */,
    //a7 though is the EID
    uint32_t EID
) {
    dputs("Recieved S-Mode ECALL (LEGACY SBI Call)");
    dprintf("  a0:  0x%lX", a0);
    dprintf("  a1:  0x%lX", a1);
    dprintf("  a2:  0x%lX", a2);
    dprintf("  a3:  0x%lX", a3);
    //No legacy SBI calls use a4 thru a6
    dprintf("  EID: 0x%lX", EID);

    long result = -1;//Some compilers aren't smart enough to figure out this is always initialized below...
    switch (EID) {
        case 0x00:
            dputs("LEGACY Function: sbi_set_timer()");
            uint64_t stime_value = ((uint64_t)a0) | (((uint64_t)a1) << 32);
            if (!set_timer_and_clear_pending_int(stime_value)) {
                result = -1;
            } else {
                result = 0;
            }
            break;
        case 0x01:
            dputs("LEGACY Function: sbi_console_putchar()");
            result = (putc((char)a0, stdout) == EOF) ? -1 : 0;
            break;
        case 0x02:
            dputs("LEGACY Function: sbi_console_getchar()");
            result = (long)(getc(stdin));
            if (result == EOF) {
                result = -1;
            }
            break;
        case 0x03:
            dputs("LEGACY Function: sbi_clear_ipi()");
            __asm__ volatile (//OGSBI only supports a single hart
                "csrci mip, 0b00000000000000000000000000000010"//Clear the supervisor software interrupt pending bit
                : /* No output registers */
                : /* No source registers */
                : /* No clobbered registers */
            );
            break;
        case 0x04:
            dputs("LEGACY Function: sbi_send_ipi()");
            if (virtual_read_word((const unsigned long*)a0) != 0b1) {
                result = -1;     
            } else {//OGSBI only supports a single hart
                __asm__ volatile (
                    "csrwi mip, 0b00000000000000000000000000000010"//Set the supervisor software interrupt pending bit
                    : /* No output registers */
                    : /* No source registers */
                    : /* No clobbered registers */
                );
                result = 0;
            }
            break;
        case 0x05:
            dputs("LEGACY Function: sbi_remote_fence_i()");
            if (virtual_read_word((const unsigned long*)a0) != 0b1) {
                result = -1;     
            } else {//OGSBI only supports a single hart
                __asm__ volatile (
                    "fence.i"//Flush the instruction cache
                    : /* No output registers */
                    : /* No source registers */
                    : /* No clobbered registers */
                );
                result = 0;
            }
            break;
        case 0x06:
            dputs("LEGACY Function: sbi_remote_sfence_vma()");
            if (virtual_read_word((const unsigned long*)a0) != 0b1) {
                result = -1;     
            } else {//OGSBI only supports a single hart
                //TODO be more efficient and only flush the page table entries for the given address range
                __asm__ volatile ("sfence.vma x0, x0");
                result = 0;
            }
            break;
        case 0x07:
            dputs("LEGACY Function: sbi_remote_sfence_vma_asid()");
            if (virtual_read_word((const unsigned long*)a0) != 0b1) {
                result = -1;
            } else {//OGSBI only supports a single hart
                //TODO be more efficient and only flush the page table entries for the given address range and ASID
                __asm__ volatile ("sfence.vma x0, x0");
                result = 0;
            }
            break;
        case 0x08:
            dputs("LEGACY Function: sbi_shutdown()");
            dputs("  Shutting down, au revoir! ...");
            exit(0);
            break;
        case 0x09 ... 0x0F:
            dputs("Invalid or unsupported LEGACY SBI extension!");
            result = -1;//Best guess for an error code (not that we expect any more LEGACY SBI calls to be defined in the future)
            break;
        default:
            assert(false && "We should never get here (the new handler should have been called instead)!");
            exit(1);
            break;
    }

    return result;
}
