/* common.h
 * By: John Jekel
 *
 * Common things for
 *
 * Based on code from rv32esim
*/

#ifndef COMMON_H
#define COMMON_H

/* Includes */

#include <stdint.h>

/* Macros */

#define SIGN_EXTEND_TO_32(data, num_bits) (((uint32_t)(((int32_t)((((uint32_t)data) << (32 - num_bits)) & (1ull << 31))) >> (31 - num_bits))) | ((uint32_t)data))

/* Constants And Defines */

//TODO

/* Types */

//TODO

/* Global Variables */

//TODO

/* Function/Class Declarations */

//TODO

#endif//COMMON_H
