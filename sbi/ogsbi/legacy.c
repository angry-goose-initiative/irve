/* legacy.c
 * Copyright (C) 2023 John Jekel
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Code to handle legacy SBI calls
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
long handle_legacy_sbi_smode_ecall(
    uint32_t a0  __attribute__((unused)), 
    uint32_t a1  __attribute__((unused)),
    uint32_t a2  __attribute__((unused)),
    uint32_t a3  __attribute__((unused)),
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

/* Static Function Implementations */

//TODO
