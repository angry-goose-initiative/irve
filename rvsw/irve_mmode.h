/**
 * @file    irve_mmode.h
 * @brief   MMIO/etc for RISC-V Machine Mode programs running on irve
 * 
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  See the LICENSE file at the root of the project for licensing info.
 * 
 * TODO longer description
 *
*/

#ifndef IRVE_MMODE_H
#define IRVE_MMODE_H

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include <stdbool.h>

/* ------------------------------------------------------------------------------------------------
 * Constants/Defines
 * --------------------------------------------------------------------------------------------- */

//These only work in M-Mode

#define IRVE_MMODE_DEBUG_ADDR (*(volatile char*)(0xFFFFFFFF))

//The while(true) convinces the compiler that this is a dead end,
//but the custom instruction is what actually stops the program
#define irve_mmode_exit() do { \
    __asm__ volatile (".insn r CUSTOM_0, 0, 0, zero, zero, zero"); \
    while (true); \
} while (0)

#endif//IRVE_MMODE_H
