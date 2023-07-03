/* memory_map.h
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * The memory map for IRVE
 * 
*/

#ifndef MEMORY_MAP_H
#define MEMORY_MAP_H

// Region for main memory
#define MEM_MAP_REGION_START_RAM    0x00000000
#define MEM_MAP_REGION_END_RAM      0x03FFFFFF

// Region for memory mapped registers
#define MEM_MAP_REGION_START_MMCSR  0xFFFFFFE0
#define MEM_MAP_ADDR_MTIME          0xFFFFFFE0
#define MEM_MAP_ADDR_MTIMEH         0xFFFFFFE4
#define MEM_MAP_ADDR_MTIMECMP       0xFFFFFFE8
#define MEM_MAP_ADDR_MTIMECMPH      0xFFFFFFEC
#define MEM_MAP_REGION_END_MMCSR    0xFFFFFFEF

// RISC-V code that writes a series of bytes to this address will print them to stdout (flushed
// when a newline is encountered)
#define MEM_MAP_ADDR_DEBUG          0xFFFFFFFF

#endif//MEMORY_MAP_H