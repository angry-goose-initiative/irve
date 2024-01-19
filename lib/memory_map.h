/**
 * @file    memory_map.h
 * @brief   The memory map for IRVE
 * 
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  Copyright (C) 2023 Nick Chan\n
 *  See the LICENSE file at the root of the project for licensing info.
*/

/* ------------------------------------------------------------------------------------------------
 * Constants/Defines
 * --------------------------------------------------------------------------------------------- */

#ifndef MEMORY_MAP_H
#define MEMORY_MAP_H

//Region for user memory
#define MEM_MAP_REGION_START_USER_RAM       (uint64_t)0x00000000
#define MEM_MAP_REGION_END_USER_RAM         (uint64_t)0x03FFFFFF

#define MEM_MAP_REGION_SIZE_USER_RAM        (MEM_MAP_REGION_END_USER_RAM - MEM_MAP_REGION_START_USER_RAM + 1)

//Region for kernel memory
#define MEM_MAP_REGION_START_KERNEL_RAM     (uint64_t)0xC0000000
#define MEM_MAP_REGION_END_KERNEL_RAM       (uint64_t)0xC3FFFFFF

#define MEM_MAP_REGION_SIZE_KERNEL_RAM      (MEM_MAP_REGION_END_KERNEL_RAM - MEM_MAP_REGION_START_KERNEL_RAM + 1)

//Region for memory mapped registers
#define MEM_MAP_REGION_START_MMCSR          (uint64_t)0xFFFFFFE0
#define MEM_MAP_ADDR_MTIME                  (uint64_t)0xFFFFFFE0
#define MEM_MAP_ADDR_MTIMEH                 (uint64_t)0xFFFFFFE4
#define MEM_MAP_ADDR_MTIMECMP               (uint64_t)0xFFFFFFE8
#define MEM_MAP_ADDR_MTIMECMPH              (uint64_t)0xFFFFFFEC
#define MEM_MAP_REGION_END_MMCSR            (uint64_t)0xFFFFFFEF

#define MEM_MAP_REGION_SIZE_MMCSR           (MEM_MAP_REGION_END_MMCSR - MEM_MAP_REGION_START_MMCSR + 1)

//Region for 16550 UART
#define MEM_MAP_REGION_START_UART           (uint64_t)0xFFFFFFF0
#define MEM_MAP_REGION_END_UART             (uint64_t)0xFFFFFFF7

//Debug output
#define MEM_MAP_ADDR_DEBUG                  (uint64_t)0xFFFFFFFF

#endif//MEMORY_MAP_H
