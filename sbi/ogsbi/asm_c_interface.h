/* asm_c_interface.h
 * Copyright (C) 2023 John Jekel
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Assembly things exposed to C code
 *
*/

#ifndef ASM_C_INTERFACE_H
#define ASM_C_INTERFACE_H

/* Includes */

#include <stdint.h>

/* Constants And Defines */

//SBI Constants
#define SBI_SUCCESS                 0
#define SBI_ERR_FAILED              -1
#define SBI_ERR_NOT_SUPPORTED       -2
#define SBI_ERR_INVALID_PARAM       -3
#define SBI_ERR_DENIED              -4
#define SBI_ERR_INVALID_ADDRESS     -5
#define SBI_ERR_ALREADY_AVAILABLE   -6
#define SBI_ERR_ALREADY_STARTED     -7
#define SBI_ERR_ALREADY_STOPPED     -8
#define SBI_ERR_INVALID_HART_ID     -9

/* Types */

typedef struct {
    long error;//a0
    long value;//a1
} sbiret_t;

/* Global Variables */

//TODO

/* Function/Class Declarations */

void jump2linux(uint32_t hart_id, uint32_t dtb_addr, uint32_t kernel_addr) __attribute__((noreturn));

/* Function/Class Definitions For C code to implement */

sbiret_t handle_smode_ecall(
    uint32_t a0  __attribute__((unused)), 
    uint32_t a1  __attribute__((unused)),
    uint32_t a2  __attribute__((unused)),
    uint32_t a3  __attribute__((unused)),
    uint32_t a4  __attribute__((unused)),
    uint32_t a5  __attribute__((unused)),
    uint32_t FID __attribute__((unused)),
    uint32_t EID __attribute__((unused))
);

void handle_other_exceptions(uint32_t registers[31], uint32_t mcause, uint32_t mepc);

#endif//ASM_C_INTERFACE_H
