/**
 * @file    morevm.cpp
 * @brief   More virtual memory experiments!
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
#include <cstdlib>
#include <utility>

/* ------------------------------------------------------------------------------------------------
 * Static Variables
 * --------------------------------------------------------------------------------------------- */

static volatile uint32_t root_page_table[1024]              __attribute__((aligned(4096)));
static volatile uint32_t one_second_level_page_table[1024]  __attribute__((aligned(4096)));

static volatile struct {
    //We initialize these with something non-zero so they aren't stored in the .bss section
    //Otherwise it can take a long time to clear them all, and we're going to fill them with random data anyway later
    volatile uint8_t a[4096] __attribute__((aligned(4096))) = {1};
    volatile uint8_t b[4096] __attribute__((aligned(4096))) = {2};
    volatile uint8_t c[4096] __attribute__((aligned(4096))) = {3};
    volatile uint8_t d[4096] __attribute__((aligned(4096))) = {4};
} page __attribute__((aligned(4194304)));//So we only need a single second-level page table (align to a superpage)

/* ------------------------------------------------------------------------------------------------
 * Static Function Declarations
 * --------------------------------------------------------------------------------------------- */

static void setup_paging();
static void switch_to_partial_two_level_paging();
static void swap_around_pages_abcd();
static void make_everything_a();
static std::pair<uint16_t, uint16_t> get_ppn_containing(volatile void* addr);

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

int main(int, const char**) {
    uint32_t seed = 0x1234ABCD;
    std::srand(seed);
    std::cout << "More Virtual Memory Experiments (seed " << std::hex << seed << std::dec << ")" << std::endl << std::endl;

    std::cout << "Let's initialize pages a, b, c, and d" << std::endl;
    for (uint32_t i = 0; i < 4096; ++i) {
        uint32_t random_word = std::rand();
        page.a[i] = random_word & 0xFF;
        page.b[i] = (random_word >> 8) & 0xFF;
        page.c[i] = (random_word >> 16) & 0xFF;
        page.d[i] = (random_word >> 24) & 0xFF;
    }
    std::cout << "Done!" << std::endl;

    std::cout << "Next, let's setup paging (simply mapping all of virtual memory to all of physical memory)" << std::endl;
    setup_paging();
    std::cout << "Alrighty, we survived that :)" << std::endl;

    std::cout << "Do each of the pages still contain what they should?" << std::endl;
    std::srand(seed);
    for (uint32_t i = 0; i < 4096; ++i) {
        uint32_t random_word = std::rand();
        assert(page.a[i] == (random_word & 0xFF));
        assert(page.b[i] == ((random_word >> 8) & 0xFF));
        assert(page.c[i] == ((random_word >> 16) & 0xFF));
        assert(page.d[i] == ((random_word >> 24) & 0xFF));
    }
    std::cout << "Yep!" << std::endl;

    std::cout << "Next, let's replace one superpage with a second-level page table" << std::endl;
    switch_to_partial_two_level_paging();
    std::cout << "Ha, ha, ha, ha, staying alive, staying alive..." << std::endl;

    std::cout << "Do each of the pages still contain what they should?" << std::endl;
    std::srand(seed);
    for (uint32_t i = 0; i < 4096; ++i) {
        uint32_t random_word = std::rand();
        assert(page.a[i] == (random_word & 0xFF));
        assert(page.b[i] == ((random_word >> 8) & 0xFF));
        assert(page.c[i] == ((random_word >> 16) & 0xFF));
        assert(page.d[i] == ((random_word >> 24) & 0xFF));
    }
    std::cout << "Yep!" << std::endl;

    std::cout << "Let's move the pages around a bit with virtual memory!" << std::endl;
    swap_around_pages_abcd();
    std::cout << "Done!" << std::endl;

    std::cout << "Were the pages swapped correctly?" << std::endl;
    std::srand(seed);
    for (uint32_t i = 0; i < 4096; ++i) {
        uint32_t random_word = std::rand();
        //Trying to access a will give us b, b will give us c, c will give us d, and d will give us a
        //This is despite the fact that the addresses haven't changed at all!
        assert(page.d[i] == (random_word & 0xFF));
        assert(page.a[i] == ((random_word >> 8) & 0xFF));
        assert(page.b[i] == ((random_word >> 16) & 0xFF));
        assert(page.c[i] == ((random_word >> 24) & 0xFF));
    }
    std::cout << "Yep!" << std::endl;

    std::cout << "Let's make everything point to page a" << std::endl;
    make_everything_a();
    std::cout << "Done!" << std::endl;

    std::cout << "Did that work?" << std::endl;
    std::srand(seed);
    for (uint32_t i = 0; i < 4096; ++i) {
        uint32_t random_word = std::rand();
        assert(page.a[i] == (random_word & 0xFF));
        assert(page.b[i] == (random_word & 0xFF));
        assert(page.c[i] == (random_word & 0xFF));
        assert(page.d[i] == (random_word & 0xFF));
    }
    std::cout << "Yep!" << std::endl;

    uint32_t new_seed = seed * 1234;
    std::cout << "Overwriting page.c, which actually points to page a, with a new seed (0x" << std::hex << new_seed << std::dec << ")" << std::endl;
    std::cout << "This also checks that, even though page.a is mapped as read only, since page.c is mapped RW, we can still write to it" << std::endl;
    std::srand(new_seed);
    for (uint32_t i = 0; i < 4096; ++i) {
        uint32_t random_word = std::rand();
        page.c[i] = random_word & 0xFF;
    }
    std::cout << "Done!" << std::endl;

    std::cout << "Disabling paging" << std::endl;
    __asm__ volatile ("csrw satp, zero");
    __asm__ volatile ("sfence.vma");
    std::cout << "Done!" << std::endl;

    std::cout << "Ensuring that only page.a was overwritten, and all the other pages contain their original initialized contents" << std::endl;
    std::srand(seed);
    for (uint32_t i = 0; i < 4096; ++i) {
        uint32_t random_word = std::rand();
        //Not checking a yet since it was changed
        assert(page.b[i] == ((random_word >> 8) & 0xFF));
        assert(page.c[i] == ((random_word >> 16) & 0xFF));
        assert(page.d[i] == ((random_word >> 24) & 0xFF));
    }
    std::srand(new_seed);
    for (uint32_t i = 0; i < 4096; ++i) {
        uint32_t random_word = std::rand();
        assert(page.a[i] == (random_word & 0xFF));
    }
    std::cout << "Yep!" << std::endl;

    std::cout << "We made it to the end! :D" << std::endl;
    return 0;
}

extern "C" __attribute__ ((interrupt ("supervisor"))) void ___rvsw_exception_handler___(void) {
    assert(false && "We don't expect any exceptions to occur in this test program");
    exit(1);
}

/* ------------------------------------------------------------------------------------------------
 * Static Function Implementations
 * --------------------------------------------------------------------------------------------- */

static void setup_paging() {
    //Begin by mapping all of memory to itself (pseudo-Bare mode)
    for (uint32_t i = 0; i < 1024; ++i) {
        //0x000000CF means R/W/X and valid (0xF), and Dirty + Accessed (0xC)
        //R/W/X and valid is self-explanatory
        //Dirty and Accessed is just to make sure we don't get any page faults (since the hardware cause
        //an exception in order to let the kernel approximate LRU)
        //Lastly NOT G or U since SUM is not set and only one ASID really
        root_page_table[i] = (i << 20) | 0x000000CF;
    }

    __asm__ volatile ("sfence.vma x0, x0");

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

static void switch_to_partial_two_level_paging() {
    std::cout << "Address of page_a: 0x" << std::hex << (uint32_t)page.a << std::dec << std::endl;
    std::cout << "Address of page_b: 0x" << std::hex << (uint32_t)page.b << std::dec << std::endl;
    std::cout << "Address of page_c: 0x" << std::hex << (uint32_t)page.c << std::dec << std::endl;
    std::cout << "Address of page_d: 0x" << std::hex << (uint32_t)page.d << std::dec << std::endl;
    auto ppn_a = get_ppn_containing(&page.a);
    auto ppn_b = get_ppn_containing(&page.b);
    auto ppn_c = get_ppn_containing(&page.c);
    auto ppn_d = get_ppn_containing(&page.d);
    std::cout << "PPN of page_a: 1: 0x" << std::hex << ppn_a.first << ", 2: 0x" << ppn_a.second << std::dec << std::endl;
    std::cout << "PPN of page_b: 1: 0x" << std::hex << ppn_b.first << ", 2: 0x" << ppn_b.second << std::dec << std::endl;
    std::cout << "PPN of page_c: 1: 0x" << std::hex << ppn_c.first << ", 2: 0x" << ppn_c.second << std::dec << std::endl;
    std::cout << "PPN of page_d: 1: 0x" << std::hex << ppn_d.first << ", 2: 0x" << ppn_d.second << std::dec << std::endl;

    //These should be guaranteed to be the same based on how we specified the alignment in the struct definition
    assert(ppn_a.first == ppn_b.first);
    assert(ppn_b.first == ppn_c.first);
    assert(ppn_c.first == ppn_d.first);
    uint32_t common_ppn1 = ppn_a.first;

    //In the one second level page table we have, also have the pages mapped to themselves
    //std::memset(one_second_level_page_table, 0, sizeof(one_second_level_page_table));//To avoid bugs
    for (uint32_t i = 0; i < 1024; ++i) {
        //0x000000CF means R/W/X and valid (0xF), and Dirty + Accessed (0xC)
        one_second_level_page_table[i] = (common_ppn1 << 20) | (i << 10) | 0x000000CF;
    }
    
    //Also actually add the second level page table to the root page table
    uint32_t one_second_level_page_table_ppn10 = ((uint32_t)one_second_level_page_table) >> 12;
    root_page_table[common_ppn1] = (one_second_level_page_table_ppn10 << 10) | 0x00000001;

    //After changing the page tables, we need to flush the TLB
    __asm__ volatile ("sfence.vma x0, x0");
}

static void swap_around_pages_abcd() {
    auto ppn_a = get_ppn_containing(&page.a);
    auto ppn_b = get_ppn_containing(&page.b);
    auto ppn_c = get_ppn_containing(&page.c);
    auto ppn_d = get_ppn_containing(&page.d);

    //These should be guaranteed to be the same based on how we specified the alignment in the struct definition
    assert(ppn_a.first == ppn_b.first);
    assert(ppn_b.first == ppn_c.first);
    assert(ppn_c.first == ppn_d.first);
    uint32_t common_ppn1 = ppn_a.first;

    //We do a very simple swap. a -> b, b -> c, c -> d, d -> a
    one_second_level_page_table[ppn_a.second] = (common_ppn1 << 20) | (ppn_b.second << 10) | 0x00000043;
    one_second_level_page_table[ppn_b.second] = (common_ppn1 << 20) | (ppn_c.second << 10) | 0x00000043;
    one_second_level_page_table[ppn_c.second] = (common_ppn1 << 20) | (ppn_d.second << 10) | 0x00000043;
    one_second_level_page_table[ppn_d.second] = (common_ppn1 << 20) | (ppn_a.second << 10) | 0x00000043;

    //After changing the page tables, we need to flush the TLB
    __asm__ volatile ("sfence.vma x0, x0");
}

static void make_everything_a() {
    auto ppn_a = get_ppn_containing(&page.a);
    auto ppn_b = get_ppn_containing(&page.b);
    auto ppn_c = get_ppn_containing(&page.c);
    auto ppn_d = get_ppn_containing(&page.d);

    //These should be guaranteed to be the same based on how we specified the alignment in the struct definition
    assert(ppn_a.first == ppn_b.first);
    assert(ppn_b.first == ppn_c.first);
    assert(ppn_c.first == ppn_d.first);
    uint32_t common_ppn1 = ppn_a.first;

    //All of these pages are in the same superpage. We simply make ppn0 for all of them point to a
    one_second_level_page_table[ppn_a.second] = (common_ppn1 << 20) | (ppn_a.second << 10) | 0x00000043;
    one_second_level_page_table[ppn_b.second] = (common_ppn1 << 20) | (ppn_a.second << 10) | 0x00000043;
    one_second_level_page_table[ppn_c.second] = (common_ppn1 << 20) | (ppn_a.second << 10) | 0x000000C7;//Make page.c writable and mark it as already dirty
    one_second_level_page_table[ppn_d.second] = (common_ppn1 << 20) | (ppn_a.second << 10) | 0x00000043;

    //After changing the page tables, we need to flush the TLB
    __asm__ volatile ("sfence.vma x0, x0");
}

static std::pair<uint16_t, uint16_t> get_ppn_containing(volatile void* addr) {
    return std::pair((uint16_t)(((uint32_t)addr) >> 22), (uint16_t)(((uint32_t)addr) >> 12) & 0x3FF);
}
