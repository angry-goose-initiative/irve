/**
 * @file    irve_vm_bringup.cpp
 * @brief   A testfile used for virtual memory bringup.
 * 
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  See the LICENSE file at the root of the project for licensing info.
*/

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include <cassert>
#include <iostream>
#include <iomanip>
#include <cstdint>
#include <cstring>

/* ------------------------------------------------------------------------------------------------
 * Static Variables
 * --------------------------------------------------------------------------------------------- */

static volatile uint32_t root_page_table[1024] __attribute__((aligned(4096)));

/* ------------------------------------------------------------------------------------------------
 * Static Function Declarations
 * --------------------------------------------------------------------------------------------- */

static void setup_page_table();
static void enable_paging();

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

int main(int, const char**) {
    std::cout << "IRVE Virtual Memory Bringup/Experiments" << std::endl;

    std::cout << "We are currently in Bare mode with regards to virtual memory" << std::endl;
    std::cout << "Let's get things set up to switch to Sv32 mode" << std::endl;

    setup_page_table();

    std::cout << "Enabling Sv32 paging" << std::endl;
    enable_paging();
    std::cout << "Did we make it here?" << std::endl;

    return 0;
}

extern "C" __attribute__ ((interrupt ("supervisor"))) void ___rvsw_exception_handler___(void) {
    assert(false && "We don't expect any exceptions to occur in this test program");
    exit(1);
}

/* ------------------------------------------------------------------------------------------------
 * Static Function Implementations
 * --------------------------------------------------------------------------------------------- */

static void setup_page_table() {
    //TESTING
    //std::memset(root_page_table, 0, sizeof(root_page_table));

    //Just map all of memory to itself (pseudo-Bare mode)
    for (uint32_t i = 0; i < 1024; ++i) {
        //0x000000CF means R/W/X and valid (0xF), and Dirty + Accessed (0xC)
        //R/W/X and valid is self-explanatory
        //Dirty and Accessed is just to make sure we don't get any page faults (since the hardware cause
        //an exception in order to let the kernel approximate LRU)
        //Lastly NOT G or U since SUM is not set and only one ASID really
        root_page_table[i] = (i << 20) | 0x000000CF;
    }

    __asm__ volatile ("sfence.vma x0, x0");
}

static void enable_paging() {
    uint32_t new_satp = 0x80000000 | (((uint32_t)root_page_table) >> 12);//ASID of 0 too
    std::cout << "new_satp: " << std::hex << new_satp << std::dec << std::endl;

    __asm__ volatile (
        "csrw satp, %[rs1]"
        : /* No destination registers */
        : [rs1] "r" (new_satp)
        : /* No clobbered registers */
    );

    __asm__ volatile ("sfence.vma x0, x0");
}
