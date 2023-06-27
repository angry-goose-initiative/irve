/* sbi.c
 * Copyright (C) 2023 John Jekel
 * See the LICENSE file at the root of the project for licensing info.
 *
 *
 * Code to handle "new" SBI calls
 *
*/

/* Constants And Defines */

//TODO

/* Includes */

#include "asm_c_interface.h"
#include "common.h"

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/* Types */

//TODO

/* Variables */

//TODO

/* Static Function Declarations */

//TODO

/* Function Implementations */

//TODO remove the __attribute__((unused))s
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
    dprintf("  a0:  0x%lX", a0);
    dprintf("  a1:  0x%lX", a1);
    dprintf("  a2:  0x%lX", a2);
    dprintf("  a3:  0x%lX", a3);
    dprintf("  a4:  0x%lX", a4);
    dprintf("  a5:  0x%lX", a5);
    dprintf("  FID: 0x%lX", FID);
    dprintf("  EID: 0x%lX", EID);

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
                    dprintf("  SBI Spec Version: 0x%lX", result.value);
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
                        case 0x00 ... 0x08: result.value = 1; break;//All presently defined legacy functions are supported
                        case 0x10:          result.value = 1; break;//Base Extension
                        case 0x4442434E:    result.value = 1; break;//Debug Console Extension
                        default:            result.value = 0; break;//Unsupported or non-existent extension
                    }
                    dprintf("  Extension 0x%lX is %s", a0, (result.value == 1) ? "supported" : "unsupported");
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
                    dprintf("  mvendorid: 0x%lX", result.value);
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
                    dprintf("  marchid: 0x%lX", result.value);
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
                    dprintf("  mimpid: 0x%lX", result.value);
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

/* Static Function Implementations */

//TODO
