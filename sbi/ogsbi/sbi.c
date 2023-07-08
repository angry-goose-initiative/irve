/**
 * @file    sbi.c
 * @brief   Code to handle "new" SBI calls
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
sbiret_t handle_sbi_smode_ecall(
    uint32_t a0,
    uint32_t a1,
    uint32_t a2 __attribute__((unused)),
    uint32_t a3 __attribute__((unused)),
    uint32_t a4 __attribute__((unused)),
    uint32_t a5 __attribute__((unused)),
    uint32_t FID,
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
                        case 0x48534D:      result.value = 1; break;//Hart State Management Extension
                        case 0x504D55:      result.value = 1; break;//Performance Monitoring Unit Extension
                        case 0x735049:      result.value = 1; break;//IPI Extension
                        case 0x4442434E:    result.value = 1; break;//Debug Console Extension
                        case 0x52464E43:    result.value = 1; break;//RFENCE Extension
                        case 0x53525354:    result.value = 1; break;//System Reset Extension
                        case 0x53555350:    result.value = 1; break;//System Suspend Extension
                        case 0x54494D45:    result.value = 1; break;//Timer Extension
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
        case 0x48534D:
            dputs("Hart State Management Extension");
            assert(false && "TODO implement");
            break;
        case 0x504D55:
            dputs("Performance Monitoring Unit Extension");
            assert(false && "TODO implement");
            break;
        case 0x735049:
            dputs("IPI Extension");
            switch (FID) {
                case 0:
                    dputs("Function: sbi_send_ipi()");
                    assert(false && "TODO implement");
                    break;
                default:
                    dputs("Invalid or unsupported IPI Extension function!");
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
                        char c = virtual_read_byte(((unsigned char*)a1) + i);
                        putc(c, stdout);
                    }
                    result.error = SBI_SUCCESS;
                    result.value = a0;
                    break;
                case 1:
                    dputs("Function: sbi_debug_console_read()");
                    //TODO handle the case where base_addr is not valid (including when base_addr_hi is any non-zero value)
                    for (size_t i = 0; i < a0; ++i) {
                        char c = getc(stdin);
                        virtual_write_byte(((unsigned char*)a1) + i, c);
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
                    dputs("Invalid or unsupported Debug Console Extension function!");
                    result.error = SBI_ERR_NOT_SUPPORTED;
                    break;
            }
            break;
        case 0x52464E43:
            dputs("RFENCE Extension");
            assert(false && "TODO implement");
            break;
        case 0x53525354:
            dputs("System Reset Extension");
            assert(false && "TODO implement");
            break;
        case 0x53555350:
            dputs("System Suspend Extension");
            assert(false && "TODO implement");
            break;
        case 0x54494D45:
            dputs("Timer Extension");
            switch (FID) {
                case 0:
                    dputs("Function: sbi_set_timer()");
                    uint64_t stime_value = ((uint64_t)a0) | (((uint64_t)a1) << 32);
                    if (!set_timer_and_clear_pending_int(stime_value)) {
                        result.error = SBI_ERR_FAILED;
                    } else {
                        result.error = SBI_SUCCESS;
                    }
                    break;
                default:
                    dputs("Invalid or unsupported Timer Extension function!");
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
