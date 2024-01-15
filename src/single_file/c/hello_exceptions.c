/**
 * @file    hello_exceptions.c
 * @brief   Early experiments with RISC-V exception handling
 * 
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  See the LICENSE file at the root of the project for licensing info.
*/

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

int main() {
    puts("Playing with RISC-V exceptions!");

    __asm__ volatile("ecall");

    puts("Back in main!");

    return 0;//This should be a regular return
}

//Adding this attribute makes the compiler emit a different return instruction
//It will also save all registers to the stack to avoid clobbering them
__attribute__ ((interrupt ("machine"))) void ___rvsw_exception_handler___(void) {
    puts("Hey would you look at that, an exception!");

    //We want to simply go to the instruction after the ecall
    uint32_t pc_of_ecall;
    __asm__ volatile (//Read mepc without modifying it
        "csrrsi %[rd], mepc, 0"
        : [rd] "=r" (pc_of_ecall)
        : /* No source registers */
        : /* No clobbered registers */
    );
    uint32_t pc_to_return_to = pc_of_ecall + 4;//The instruction after the ecall
    __asm__ volatile (//Update mepc
        "csrrw %[rd], mepc, %[rs1]"
        : [rd] "=r" (pc_to_return_to)
        : [rs1] "r" (pc_to_return_to)
        : /* No clobbered registers */
    );
    assert(pc_to_return_to == pc_of_ecall);//The swap worked

    return;//This should be different than a normal return when emitted by the compiler (MRET)
}
