/**
 * @file    common.h
 * @brief   Common things for OGSBI
 * 
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  See the LICENSE file at the root of the project for licensing info.
 * 
 * TODO longer description
 *
*/

#ifndef COMMON_H
#define COMMON_H

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include <stdbool.h>
#include <stdint.h>

/* ------------------------------------------------------------------------------------------------
 * Constants/Defines
 * --------------------------------------------------------------------------------------------- */

#ifndef NDEBUG

#define dputc(c) fputc(c, stderr)

#define dputs(str) fputs("OGSBI> " str "\n", stderr)

#define dprintf(...) do { \
    fprintf(stderr, "OGSBI> " __VA_ARGS__); \
    fputc('\n', stderr); \
} while (0)

#else
#define dputc(c) do {} while (0)
#define dputs(str) do {} while (0)
#define dprintf(...) do {} while (0)
#endif

/* ------------------------------------------------------------------------------------------------
 * Function Declarations
 * --------------------------------------------------------------------------------------------- */

bool set_timer_and_clear_pending_int(uint64_t stime_value);//Returns false if there was an error

#endif//COMMON_H
