/**
 * @brief   The memory map for IRVE
 * 
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  Copyright (C) 2023 Nick Chan\n
 *  See the LICENSE file at the root of the project for licensing info.
*/

#pragma once

//Region for user memory
#define MEM_MAP_REGION_START_USER_RAM       (uint64_t)0x00000000
#define MEM_MAP_REGION_END_USER_RAM         (uint64_t)0x03FFFFFF

#define MEM_MAP_REGION_SIZE_USER_RAM        (MEM_MAP_REGION_END_USER_RAM - MEM_MAP_REGION_START_USER_RAM + 1)

//Region for kernel memory
#define MEM_MAP_REGION_START_KERNEL_RAM     (uint64_t)0x80000000
#define MEM_MAP_REGION_END_KERNEL_RAM       (uint64_t)0x83FFFFFF

#define MEM_MAP_REGION_SIZE_KERNEL_RAM      (MEM_MAP_REGION_END_KERNEL_RAM - MEM_MAP_REGION_START_KERNEL_RAM + 1)

//Region for memory mapped registers
#define MEM_MAP_REGION_START_ACLINT         (uint64_t)0xF0000000
#define MEM_MAP_REGION_END_ACLINT           (uint64_t)0xF000BFFF

#define MEM_MAP_REGION_SIZE_ACLINT           (MEM_MAP_REGION_END_ACLINT - MEM_MAP_REGION_START_ACLINT + 1)

//Region for 16550 UART
#define MEM_MAP_REGION_START_UART           (uint64_t)0xF1000000
#define MEM_MAP_REGION_END_UART             (uint64_t)0xF1000007

//Debug output
#define MEM_MAP_ADDR_DEBUG                  (uint64_t)0xFFFFFFFF
