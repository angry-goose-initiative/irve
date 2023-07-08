/**
 * @file    asm_c_interface.h
 * @brief   Assembly things exposed to C code
 * 
 * @copyright Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 * 
 * TODO longer description
 *
*/

#ifndef ASM_C_INTERFACE_H
#define ASM_C_INTERFACE_H

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include <stdint.h>

/* ------------------------------------------------------------------------------------------------
 * Constants/Defines
 * --------------------------------------------------------------------------------------------- */

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

/* ------------------------------------------------------------------------------------------------
 * Types
 * --------------------------------------------------------------------------------------------- */

typedef struct {
    long error;//a0
    long value;//a1
} sbiret_t;

/* ------------------------------------------------------------------------------------------------
 * Function Declarations
 * --------------------------------------------------------------------------------------------- */

void jump2linux(uint32_t hart_id, uint32_t dtb_addr, uint32_t kernel_addr) __attribute__((noreturn));

uint8_t  virtual_read_byte           (const uint8_t*  virtual_addr);//NOT PHYSICAL (unless S-mode is Bare, but this is handled in the function)
uint16_t virtual_read_halfword       (const uint16_t* virtual_addr);//NOT PHYSICAL (unless S-mode is Bare, but this is handled in the function)
uint32_t virtual_read_word           (const uint32_t* virtual_addr);//NOT PHYSICAL (unless S-mode is Bare, but this is handled in the function)
int8_t   virtual_read_signed_byte    (const int8_t*   virtual_addr);//NOT PHYSICAL (unless S-mode is Bare, but this is handled in the function)
int16_t  virtual_read_signed_halfword(const int16_t*  virtual_addr);//NOT PHYSICAL (unless S-mode is Bare, but this is handled in the function)

void virtual_write_byte    (uint8_t*  virtual_addr, uint8_t  value);//NOT PHYSICAL (unless S-mode is Bare, but this is handled in the function)
void virtual_write_halfword(uint16_t* virtual_addr, uint16_t value);//NOT PHYSICAL (unless S-mode is Bare, but this is handled in the function)
void virtual_write_word    (uint32_t* virtual_addr, uint32_t value);//NOT PHYSICAL (unless S-mode is Bare, but this is handled in the function)

/* Function/Class Definitions For C code to implement */

sbiret_t handle_sbi_smode_ecall(
    uint32_t a0, 
    uint32_t a1,
    uint32_t a2,
    uint32_t a3,
    uint32_t a4,
    uint32_t a5,
    uint32_t FID,
    uint32_t EID
);

long handle_legacy_sbi_smode_ecall(
    uint32_t a0,
    uint32_t a1,
    uint32_t a2,
    uint32_t a3,
    uint32_t a4,
    uint32_t a5,
    uint32_t a6,
    uint32_t EID
);

void handle_other_exceptions(uint32_t registers[31], uint32_t mcause, uint32_t mepc);

#endif//ASM_C_INTERFACE_H
