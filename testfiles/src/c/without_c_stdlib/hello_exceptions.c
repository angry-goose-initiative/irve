/* hello_exceptions.c
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Early experiments with RISC-V exception handling
 *
*/

/* Includes */

#include "irve.h"

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>

/* Function Implementations */

int main() {
    puts("Playing with RISC-V exceptions!\n");

    __asm__ volatile("ecall");

    puts("Back in main!\n");

    return 0;//This should be a regular return
}

//Adding this attribute makes the compiler emit a different return instruction
//It will also save all registers to the stack to avoid clobbering them
__attribute__ ((interrupt ("machine"))) void __riscv_synchronous_exception_and_user_mode_swi_handler(void) {
    puts("Hey would you look at that, an exception!\n");
    return;//This should be different than a normal return when emitted by the compiler
}
