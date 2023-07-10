/**
 * @file    memory.cpp
 * @brief   Handles the memory of the emulator
 * 
 * @copyright Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 * 
 * TODO longer description
 *
*/

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include "memory.h"

#include <cassert>
#include <iostream>
#include <cstring>
#include <memory>
#include <fstream>
#include <string>
#include <cstdlib>

#include "CSR.h"
#include "common.h"
#include "memory_map.h"
#include "rvexception.h"
#include "fuzzish.h"

#define INST_COUNT 0 // We only log at init
#include "logging.h"

using namespace irve::internal;

/* ------------------------------------------------------------------------------------------------
 * Constants/Defines
 * --------------------------------------------------------------------------------------------- */

// The directory where test files are located
//FIXME this should be moved to config.h
#define TESTFILES_DIR   "rvsw/compiled/"

#define DATA_WIDTH_MASK 0b11

#define WORD_ADDR_MASK  (~(uint64_t)0b11)

#define MPP_M_MODE      0b11

// A RISC-V page size is 4 KiB
#define PAGESIZE        0x1000

// Current address translation scheme
// 0 = Bare (no address translation)
// 1 = SV32
#define satp_MODE       m_CSR_ref.implicit_read(CSR::address::SATP).bit(31).u
// The physical page number field of the satp CSR
#define satp_PPN        (uint64_t)m_CSR_ref.implicit_read(CSR::address::SATP).bits(21, 0).u

// Make eXecutable readable field of the mstatus CSR
#define mstatus_MXR     m_CSR_ref.implicit_read(CSR::address::MSTATUS).bit(19).u
// permit Superisor User Memory access field of the mstatus CSR
#define mstatus_SUM     m_CSR_ref.implicit_read(CSR::address::MSTATUS).bit(18).u
// Modify PriVilege field of the mstatus CSR
#define mstatus_MPRV    m_CSR_ref.implicit_read(CSR::address::MSTATUS).bit(17).u
// Previous privilige mode field of the mstatus CSR
#define mstatus_MPP     m_CSR_ref.implicit_read(CSR::address::MSTATUS).bits(12, 11).u

// The virtual page number (VPN) of a virtual address (va)
#define va_VPN(i)       (uint64_t)va.bits(21 + (10 * i), 12 + (10 * i)).u

// Full physical page number field of the page table entry
#define pte_PPN         (uint64_t)pte.bits(31, 10).u
// Upper part of the page number field of the page table entry
#define pte_PPN1        (uint64_t)pte.bits(31, 20).u
// Lower part of the page number field of the page table entry
#define pte_PPN0        pte.bits(19, 10).u
// Page dirty bit
#define pte_D           pte.bit(7).u
// Page accessed bit
#define pte_A           pte.bit(6).u
// Global mapping bit
#define pte_G           pte.bit(5).u
// Page is accessible in U-mode bit
#define pte_U           pte.bit(4).u
// Page is executable bit
#define pte_X           pte.bit(3).u
// Page is writable bit
#define pte_W           pte.bit(2).u
// Page is readable bit
#define pte_R           pte.bit(1).u
// Page table entry valid bit
#define pte_V           pte.bit(0).u

// The current privilege mode
#define CURR_PMODE      m_CSR_ref.get_privilege_mode()

// True if a virtual memory access is not allowed
#define ACCESS_NOT_ALLOWED                                                                        \
/* The page cannot be accessed if any of the following conditions is met: */                      \
    /* 1. Fetching an instruction but the page is not marked as executable */                     \
    ((access_type == AT_INSTRUCTION) && (pte_X != 1)) ||                                          \
    /* 2. Access is a store but the page is not marked as writable */                             \
    ((access_type == AT_STORE) && (pte_W != 1)) ||                                                \
    /* 3. Access is a load but the page is either not marked as readable or mstatus.MXR is set */ \
    /*    but the page isn't marked as executable */                                              \
    ((access_type == AT_LOAD) && ((pte_R != 1) && ((mstatus_MXR == 0) || (pte_X == 0)))) ||       \
    /* 4. Either the current privilege mode is S or the effective privilege mode is S with the */ \
    /*    access being a load or a store (not an instruction) and S-mode can't access U-mode */   \
    /*    pages and the page is markes as accessible in U-mode */                                 \
    (((CURR_PMODE == CSR::privilege_mode_t::SUPERVISOR_MODE) ||                                   \
        ((access_type != AT_INSTRUCTION) && (mstatus_MPP == 0b01) && (mstatus_MPRV == 1))) &&     \
        (mstatus_SUM == 0) && (pte_U == 1))

// Access types
#define AT_INSTRUCTION  0
#define AT_LOAD         1
// 2 is Reserved
#define AT_STORE        3

#define PAGE_FAULT_BASE 12

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

memory::memory_t::memory_t(CSR::CSR_t& CSR_ref):
        m_CSR_ref(CSR_ref),
        m_user_ram(new uint8_t[MEM_MAP_REGION_SIZE_USER_RAM]),
        m_kernel_ram(new uint8_t[MEM_MAP_REGION_SIZE_KERNEL_RAM]),
        m_debugstr() {

    // Check endianness of host (only little-endian hosts are supported)
    const union {uint8_t bytes[4]; uint32_t value;} host_order = {{0, 1, 2, 3}};
    assert((host_order.value == 0x03020100) && "Host endianness not supported");

    // Initialize all ram to random values
    irve_fuzzish_meminit(this->m_user_ram.get(), MEM_MAP_REGION_SIZE_USER_RAM);
    irve_fuzzish_meminit(this->m_kernel_ram.get(), MEM_MAP_REGION_SIZE_KERNEL_RAM);

    irvelog(1, "Created new Memory instance");
}

memory::memory_t::memory_t(int imagec, const char* const* imagev, CSR::CSR_t& CSR_ref):
        m_CSR_ref(CSR_ref),
        m_user_ram(new uint8_t[MEM_MAP_REGION_SIZE_USER_RAM]),
        m_kernel_ram(new uint8_t[MEM_MAP_REGION_SIZE_KERNEL_RAM]),
        m_debugstr() {

    // Check endianness of host (only little-endian hosts are supported)
    const union {uint8_t bytes[4]; uint32_t value;} host_order = {{0, 1, 2, 3}};
    assert((host_order.value == 0x03020100) && "Host endianness not supported");

    // Initialize all ram to random values
    irve_fuzzish_meminit(this->m_user_ram.get(), MEM_MAP_REGION_SIZE_USER_RAM);
    irve_fuzzish_meminit(this->m_kernel_ram.get(), MEM_MAP_REGION_SIZE_KERNEL_RAM);

    // Load memory images and throw an exception if an error occured
    image_load_status_t load_status;
    load_status = load_memory_image_files(imagec, imagev);
    if(load_status == IL_FAIL) {
        throw std::exception();
    }

    irvelog(1, "Created new Memory instance");
}

memory::memory_t::~memory_t() {
    if (this->m_debugstr.size() > 0) {
        irvelog_always_stdout(0, "\x1b[92mRV:\x1b[0m: \"\x1b[1m%s\x1b[0m\"",
                                this->m_debugstr.c_str());
    }
}

word_t memory::memory_t::instruction(word_t addr) const {
    access_status_t access_status;
    uint64_t machine_addr = translate_address(addr, AT_INSTRUCTION);

    word_t data = read_memory(machine_addr, DT_WORD, access_status);

    if((access_status == AS_VIOLATES_PMA) || (access_status == AS_VIOLATES_PMP)) {
        invoke_rv_exception(INSTRUCTION_ACCESS_FAULT);
    }

    if(access_status == AS_MISALIGNED) {
        invoke_rv_exception(INSTRUCTION_ADDRESS_MISALIGNED);
    }

    return data;
}

word_t memory::memory_t::load(word_t addr, uint8_t data_type) {
    assert((data_type <= 0b111) && "Invalid funct3");
    assert((data_type != 0b110) && "Invalid funct3");

    access_status_t access_status;
    uint64_t machine_addr = translate_address(addr, AT_LOAD);

    word_t data = read_memory(machine_addr, data_type, access_status);

    if((access_status == AS_VIOLATES_PMA) || (access_status == AS_VIOLATES_PMP)) {
        invoke_rv_exception(LOAD_ACCESS_FAULT);
    }

    if(access_status == AS_MISALIGNED) {
        invoke_rv_exception(LOAD_ADDRESS_MISALIGNED);
    }

    return data;
}

void memory::memory_t::store(word_t addr, uint8_t data_type, word_t data) {
    assert((data_type <= 0b010) && "Invalid funct3");

    access_status_t access_status;
    uint64_t machine_addr = translate_address(addr, AT_STORE);

    write_memory(machine_addr, data_type, data, access_status);

    if((access_status == AS_VIOLATES_PMA) || (access_status == AS_VIOLATES_PMP)) {
        invoke_rv_exception(STORE_OR_AMO_ACCESS_FAULT);
    }

    if(access_status == AS_MISALIGNED) {
        invoke_rv_exception(STORE_OR_AMO_ADDRESS_MISALIGNED);
    }
}

uint64_t memory::memory_t::translate_address(word_t untranslated_addr, uint8_t access_type) const {
    if(no_address_translation(access_type)) {
        irvelog(1, "No address translation");
        return (uint64_t)untranslated_addr.u;
    }
    irvelog(1, "Translating address");

    access_status_t access_status;

    // The untranslated address is a virtual address
    word_t va = untranslated_addr;

    // The address of a pte:
    //  33       12 11          2 1  0
    // |    PPN    |  va_VPN[i]  | 00 |
    // where i is the level of the page table

    // STEP 1
    // a is the PPN left shifted to its position in the pte
    uint64_t a = satp_PPN * PAGESIZE;
    uint64_t pte_addr;
    word_t pte;

    irvelog(2, "Virtual address is 0x%08X", va.u);

    int i = 1;
    while(1) {
        // STEP 2
        pte_addr = a + (va_VPN(i) * 4);
        irvelog(2, "Accessing level %d pte at level at address 0x%09X", i, pte_addr);
        pte = read_memory(pte_addr, DT_WORD, access_status);
        if(access_status != AS_OKAY) {
            irvelog(2, "Accessing the pte violated a PMA or PMP check, raising an access fault exception");
            switch(access_type) {
                case AT_INSTRUCTION:
                    invoke_rv_exception(INSTRUCTION_ACCESS_FAULT);
                    break;
                case AT_LOAD:
                    invoke_rv_exception(LOAD_ACCESS_FAULT);
                    break;
                case AT_STORE:
                    invoke_rv_exception(STORE_OR_AMO_ACCESS_FAULT);
                    break;
                default:
                    assert(false && "Should never get here");
            }
        }
        irvelog(2, "pte found = 0x%08X", pte.u);

        assert(pte_G == 0 && "Global bit was set by software (but we haven't implemented it)");

        // STEP 3
        if(pte_V == 0 || (pte_R == 0 && pte_W == 1)) {
            irvelog(2, "The pte is not valid or the page is writable and not readable, raising exception");
            invoke_rv_exception_by_num((rvexception::cause_t)(PAGE_FAULT_BASE + access_type));
        }

        // STEP 4
        if(pte_R == 1 || pte_X == 1) {
            irvelog(2, "Leaf pte found");
            break;
        }
        else {
            a = pte_PPN * PAGESIZE;
            --i;
            if(i < 0) {
                irvelog(2, "Leaf pte not found at the second level of the page table, raising exception");
                invoke_rv_exception_by_num((rvexception::cause_t)(PAGE_FAULT_BASE + access_type));
            }
        }
    }

    // STEP 5
    if(ACCESS_NOT_ALLOWED) {
        irvelog(2, "This access is not allowed, raising exception");
        invoke_rv_exception_by_num((rvexception::cause_t)(PAGE_FAULT_BASE + access_type));
    }

    // STEP 6
    if((i == 1) && (pte_PPN0 != 0)) {
        // Misaligned superpage
        irvelog(2, "Misaligned superpage, raising exception");
        invoke_rv_exception_by_num((rvexception::cause_t)(PAGE_FAULT_BASE + access_type));
    }

    // STEP 7
    if((pte_A == 0) || ((access_type == AT_STORE) && (pte_D == 0))) {
        irvelog(2, "Accessed bit not set or operation is a store and the dirty bit is not set, raising exception");
        invoke_rv_exception_by_num((rvexception::cause_t)(PAGE_FAULT_BASE + access_type));
    }

    // STEP 8
    uint64_t machine_addr = untranslated_addr.bits(11, 0).u;
    if(i == 1) {
        // Superpage translation
        machine_addr |= va_VPN(0) << 12;
        machine_addr |= pte_PPN1 << 22;
    }
    else {
        machine_addr |= pte_PPN << 12;
    }
    irvelog(2, "Translation resulted in address 0x%09X", machine_addr);

    return machine_addr;
}

bool memory::memory_t::no_address_translation(uint8_t access_type) const {
    if(mstatus_MPRV && (access_type != AT_INSTRUCTION)) {
        // The modify privilege mode flag is set and the access type is not instruction
        if(mstatus_MPP == MPP_M_MODE || (satp_MODE == 0)) {
            return true;
        }
        else {
            return false;
        }
    }
    else {
        if((CURR_PMODE == CSR::privilege_mode_t::MACHINE_MODE) || (satp_MODE == 0)) {
            return true;
        }
        else {
            return false;
        }
    }
    assert(false && "Should never get here");
    return true;
}

word_t memory::memory_t::read_memory(uint64_t addr, uint8_t data_type, access_status_t& access_status) const {
    assert(((addr & 0xFFFFFFFC00000000) == 0) && "Address should only be 34 bits!");

    irvelog(2, "Reading from machine address 0x%09X", addr);

    access_status = AS_OKAY; // Set here to avoid uninitialized warning

    word_t data = 0;

    // All regions that contain readable memory should be covered
    if(addr <= MEM_MAP_REGION_END_USER_RAM) {
        data = read_memory_region_user_ram(addr, data_type, access_status);
    }
    else if((addr >= MEM_MAP_REGION_START_KERNEL_RAM) && (addr <= MEM_MAP_REGION_END_KERNEL_RAM)) {
        data = read_memory_region_kernel_ram(addr, data_type, access_status);
    }
    else if((addr >= MEM_MAP_REGION_START_MMCSR) && (addr <= MEM_MAP_REGION_END_MMCSR)) {
        data = read_memory_region_mmcsr(addr, data_type, access_status);
    }
    else if((addr >= MEM_MAP_REGION_START_UART) && (addr <= MEM_MAP_REGION_END_UART)) {
        data = read_memory_region_uart(addr, data_type, access_status);
    }
    else {
        access_status = AS_VIOLATES_PMA;
    }
    
    if(access_status != AS_OKAY) {
        return word_t(0);
    }

    return data;

}

word_t memory::memory_t::read_memory_region_user_ram(uint64_t addr, uint8_t data_type, access_status_t& access_status) const {
    assert((addr <= MEM_MAP_REGION_END_USER_RAM) && "This should never happen");

    // Check for misaligned access
    if (((data_type & DATA_WIDTH_MASK) == DT_HALFWORD) && ((addr & 0b1) != 0)) {
        // Misaligned halfword read
        access_status = AS_MISALIGNED;
        return word_t(0);
    }
    else if ((data_type == DT_WORD) && ((addr & 0b11) != 0)) {
        // Misaligned word read
        access_status = AS_MISALIGNED;
        return word_t(0);
    }

    word_t data;
    uint64_t mem_index = addr - MEM_MAP_REGION_START_USER_RAM;
    void* mem_ptr = &(m_user_ram[mem_index]);
    switch(data_type) {
        case DT_WORD:
            data = *(uint32_t*)mem_ptr;
            break;
        case DT_UNSIGNED_HALFWORD:
            data = (uint32_t)(*(uint16_t*)mem_ptr);
            break;
        case DT_SIGNED_HALFWORD:
            data = (int32_t)(*(int16_t*)mem_ptr);
            break;
        case DT_UNSIGNED_BYTE:
            data = (uint32_t)(*(uint8_t*)mem_ptr);
            break;
        case DT_SIGNED_BYTE:
            data = (int32_t)(*(int8_t*)mem_ptr);
            break;
        default:
            assert(false && "This should never be reached");
    }

    return data;
}

word_t memory::memory_t::read_memory_region_kernel_ram(uint64_t addr, uint8_t data_type, access_status_t& access_status) const {
    assert((addr >= MEM_MAP_REGION_START_KERNEL_RAM) && (addr <= MEM_MAP_REGION_END_KERNEL_RAM) && "This should never happen");
    
    // Check for misaligned access
    if (((data_type & DATA_WIDTH_MASK) == DT_HALFWORD) && ((addr & 0b1) != 0)) {
        // Misaligned halfword read
        access_status = AS_MISALIGNED;
        return word_t(0);
    }
    else if ((data_type == DT_WORD) && ((addr & 0b11) != 0)) {
        // Misaligned word read
        access_status = AS_MISALIGNED;
        return word_t(0);
    }

    word_t data;
    uint64_t mem_index = addr - MEM_MAP_REGION_START_KERNEL_RAM;
    void* mem_ptr = &(m_user_ram[mem_index]);
    switch(data_type) {
        case DT_WORD:
            data = *(uint32_t*)mem_ptr;
            break;
        case DT_UNSIGNED_HALFWORD:
            data = (uint32_t)(*(uint16_t*)mem_ptr);
            break;
        case DT_SIGNED_HALFWORD:
            data = (int32_t)(*(int16_t*)mem_ptr);
            break;
        case DT_UNSIGNED_BYTE:
            data = (uint32_t)(*(uint8_t*)mem_ptr);
            break;
        case DT_SIGNED_BYTE:
            data = (int32_t)(*(int8_t*)mem_ptr);
            break;
        default:
            assert(false && "This should never be reached");
    }

    return data;
}

word_t memory::memory_t::read_memory_region_mmcsr(uint64_t addr, uint8_t data_type, access_status_t& access_status) const {
    assert((addr >= MEM_MAP_REGION_START_MMCSR) && (addr <= MEM_MAP_REGION_END_MMCSR) && "This should never happen");

    // These registers must be accessed as words only
    if(data_type != DT_WORD) {
        access_status = AS_VIOLATES_PMA;
        return word_t(0);
    }

    // Check for misaligned word access
    if((addr & 0b11) != 0) {
        // Misaligned word
        access_status = AS_MISALIGNED;
        return word_t(0);
    }

    uint16_t csr_num;
    switch(addr & WORD_ADDR_MASK) {
        case MEM_MAP_ADDR_MTIME:
            csr_num = CSR::address::MTIME;
            break;
        case MEM_MAP_ADDR_MTIMEH:
            csr_num = CSR::address::MTIMEH;
            break;
        case MEM_MAP_ADDR_MTIMECMP:
            csr_num = CSR::address::MTIMECMP;
            break;
        case MEM_MAP_ADDR_MTIMECMPH:
            csr_num = CSR::address::MTIMECMPH;
            break;
        default:
            assert(false && "Should never be reached");
            break;
    }
    return m_CSR_ref.implicit_read(csr_num);
}

word_t memory::memory_t::read_memory_region_uart(uint64_t addr, uint8_t data_type, access_status_t& access_status) const {
    assert((addr >= MEM_MAP_REGION_START_UART) && (addr <= MEM_MAP_REGION_END_UART) && "This should never happen");

    assert(false && "Region not implemented yet");

    // Only byte accesses allowed
    if((data_type & DATA_WIDTH_MASK) != DT_BYTE) {
        access_status = AS_VIOLATES_PMA;
        return word_t(0);
    }

    word_t data;
    switch(addr) {
        default:
            assert(false && "This should never be reached");
    }
    return data;
}

void memory::memory_t::write_memory(uint64_t addr, uint8_t data_type, word_t data, access_status_t &access_status) {
    assert(((addr & 0xFFFFFFFC00000000) == 0) && "Address should only be 34 bits!");
    
    irvelog(2, "Writing to machine address 0x%09X", addr);

    access_status = AS_OKAY; // Set here to avoid uninitialized warning

    // All regions that contain writable memory should be covered
    if(addr <= MEM_MAP_REGION_END_USER_RAM) {
        write_memory_region_user_ram(addr, data_type, data, access_status);
    }
    else if((addr >= MEM_MAP_REGION_START_KERNEL_RAM) && (addr <= MEM_MAP_REGION_END_KERNEL_RAM)) {
        write_memory_region_kernel_ram(addr, data_type, data, access_status);
    }
    else if((addr >= MEM_MAP_REGION_START_MMCSR) && (addr <= MEM_MAP_REGION_END_MMCSR)) {
        write_memory_region_mmcsr(addr, data_type, data, access_status);
    }
    else if((addr >= MEM_MAP_REGION_START_UART) && (addr <= MEM_MAP_REGION_END_UART)) {

    }
    else if(addr == MEM_MAP_ADDR_DEBUG) {
        write_memory_region_debug(addr, data_type, data, access_status);
    }
    else {
        access_status = AS_VIOLATES_PMA;
    }

}

void memory::memory_t::write_memory_region_user_ram(uint64_t addr, uint8_t data_type, word_t data, access_status_t& access_status) {
    assert((addr <= MEM_MAP_REGION_END_USER_RAM) && "This should never happen");

    // TODO PMP check for user ram access

    // Check for misaligned access
    if (((data_type & DATA_WIDTH_MASK) == DT_HALFWORD) && ((addr & 0b1) != 0)) {
        // Misaligned halfword write
        access_status = AS_MISALIGNED;
    }
    else if ((data_type == DT_WORD) && ((addr & 0b11) != 0)) {
        // Misaligned word write
        access_status = AS_MISALIGNED;
    }

    uint64_t mem_index = addr - MEM_MAP_REGION_START_USER_RAM;
    void* mem_ptr = &(m_user_ram[mem_index]);
    switch(data_type) {
        case DT_WORD:
            *(uint32_t*)mem_ptr = data.u;
            break;
        case DT_HALFWORD:
            *(uint16_t*)mem_ptr = (uint16_t)data.u;
            break;
        case DT_BYTE:
            *(uint8_t*)mem_ptr = (uint8_t)data.u;
            break;
        default:
            assert(false && "This should never be reached");
    }
}

void memory::memory_t::write_memory_region_kernel_ram(uint64_t addr, uint8_t data_type, word_t data, access_status_t& access_status) {
    assert(((addr >= MEM_MAP_REGION_START_KERNEL_RAM) && (addr <= MEM_MAP_REGION_END_KERNEL_RAM)) && "This should never happen");

    // Check for misaligned access
    if (((data_type & DATA_WIDTH_MASK) == DT_HALFWORD) && ((addr & 0b1) != 0)) {
        // Misaligned halfword write
        access_status = AS_MISALIGNED;
    }
    else if ((data_type == DT_WORD) && ((addr & 0b11) != 0)) {
        // Misaligned word write
        access_status = AS_MISALIGNED;
    }

    uint64_t mem_index = addr - MEM_MAP_REGION_START_KERNEL_RAM;
    void* mem_ptr = &(m_user_ram[mem_index]);
    switch(data_type) {
        case DT_WORD:
            *(uint32_t*)mem_ptr = data.u;
            break;
        case DT_HALFWORD:
            *(uint16_t*)mem_ptr = (uint16_t)data.u;
            break;
        case DT_BYTE:
            *(uint8_t*)mem_ptr = (uint8_t)data.u;
            break;
        default:
            assert(false && "This should never be reached");
    }
}
    
void memory::memory_t::write_memory_region_mmcsr(uint64_t addr, uint8_t data_type, word_t data, access_status_t& access_status) {
    assert(((addr >= MEM_MAP_REGION_START_MMCSR) && (addr <= MEM_MAP_REGION_END_MMCSR)) && "This should never happen");

    // These registers must be accessed as words only
    if(data_type != DT_WORD) {
        access_status = AS_VIOLATES_PMA;
        return;
    }

    // TODO PMP checks for mmcsr

    // Check for misaligned access. We only check for a misaligned word since at this point, the
    // data width has to be a word.
    if((addr & 0b11) != 0) {
        // Misaligned word
        access_status = AS_MISALIGNED;
        return;
    }

    uint16_t csr_num;
    switch(addr & WORD_ADDR_MASK) {
        case MEM_MAP_ADDR_MTIME:
            csr_num = CSR::address::MTIME;
            break;
        case MEM_MAP_ADDR_MTIMEH:
            csr_num = CSR::address::MTIMEH;
            break;
        case MEM_MAP_ADDR_MTIMECMP:
            csr_num = CSR::address::MTIMECMP;
            break;
        case MEM_MAP_ADDR_MTIMECMPH:
            csr_num = CSR::address::MTIMECMPH;
            break;
        default:
            assert(false && "Should never be reached");
            break;
    }

    m_CSR_ref.implicit_write(csr_num, data);
}

void memory::memory_t::write_memory_region_uart(uint64_t addr, uint8_t data_type, word_t data, access_status_t& access_status) {
    assert((addr >= MEM_MAP_REGION_START_UART) && (addr <= MEM_MAP_REGION_END_UART) && "This should never happen");

    assert(false && "Region not implemented yet");

    // Only byte accesses allowed
    if((data_type & DATA_WIDTH_MASK) != DT_BYTE) {
        access_status = AS_VIOLATES_PMA;
        return;
    }

    switch(addr) {
        default:
            assert(false && "This should never be reached");
            break;
    }
}

void memory::memory_t::write_memory_region_debug(uint64_t addr, uint8_t data_type, word_t data, access_status_t& access_status) {
    assert((addr == MEM_MAP_ADDR_DEBUG) && "This should never happen");

    // This region can only be written to with a byte access
    if(data_type != DT_BYTE) {
        access_status = AS_VIOLATES_PMA;
        return;
    }

    char character = (char)data.s;
    if (character == '\n') {
        // End of line; print the debug string
        irvelog_always_stdout(0, "\x1b[92mRV\x1b[0m: \"\x1b[1m%s\x1b[0m\\n\"", this->m_debugstr.c_str());
        this->m_debugstr.clear();
    }
    else if (character == '\0') {
        // Null terminator; print the debug string
        irvelog_always_stdout(0, "\x1b[92mRV\x1b[0m: \"\x1b[1m%s\x1b[0m\\0\"", this->m_debugstr.c_str());
        this->m_debugstr.clear();
    }
    else {
        this->m_debugstr.push_back(character);
    }
}

memory::image_load_status_t memory::memory_t::load_memory_image_files(int imagec, const char* const* imagev) {

    // Load each memory file
    for(int i = 0; i < imagec; ++i) {
        std::string path = imagev[i];
        if (path.find("/") == std::string::npos) {
            path = TESTFILES_DIR + path;
        }
        irvelog_always(0, "Loading memory image from file \"%s\"", path.c_str());
        image_load_status_t load_status;
        if (path.find(".vhex8")) {
            load_status = load_verilog_8(path);
        } else {//Assume it's a 32-bit verilog file
            load_status = load_verilog_32(path);
        }
        if(load_status == IL_FAIL) {
            return IL_FAIL;
        }
    }
    return IL_OKAY;
}

memory::image_load_status_t memory::memory_t::load_verilog_8(std::string image_path) {
    std::fstream fin = std::fstream(image_path);
    assert(fin && "Failed to open memory image file");

    // Read the file token by token
    uint64_t addr = 0;
    std::string token;
    while (fin >> token) {
        assert((token.length() != 0) && "This should never happen");
        if (token.at(0) == '@') { // `@` indicates a new address (ASSUMING 32-BIT WORDS)
            std::string new_addr_str = token.substr(1);
            assert((new_addr_str.length() == 8) &&
                    "Memory image file is not formatted correctly (bad address)");
            addr = std::stoul(new_addr_str, nullptr, 16);
        }
        else { // New data word (32-bit, could be an instruction or data)
            assert((token.length() == 2) &&
                    "Memory image file is not formatted correctly (bad data)");
            
            // The data word this token represents
            word_t data_word = (uint32_t)std::stoul(token, nullptr, 16);

            // Write the data word to memory and increment the address to the next word
            access_status_t access_status;
            write_memory(addr, DT_BYTE, data_word, access_status);
            if(access_status != AS_OKAY) {
                return IL_FAIL;
            }
            ++addr;
        }
    }
    return IL_OKAY;
}

memory::image_load_status_t memory::memory_t::load_verilog_32(std::string image_path) {
    std::fstream fin = std::fstream(image_path);
    assert(fin && "Failed to open memory image file");

    // Read the file token by token
    uint64_t addr = 0;
    std::string token;
    while (fin >> token) {
        assert((token.length() != 0) && "This should never happen");
        if (token.at(0) == '@') { // `@` indicates a new address (ASSUMING 32-BIT WORDS)
            std::string new_addr_str = token.substr(1);
            assert((new_addr_str.length() == 8) &&
                    "Memory image file is not formatted correctly (bad address)");
            addr = std::stoul(new_addr_str, nullptr, 16);
            addr *= 4; // This is a word address, not a byte address, so multiply by 4
        }
        else { // New data word (32-bit, could be an instruction or data)
            if (token.length() != 8) {
                irvelog(1, "Warning: 32-bit Verilog image file is not formatted correctly (data "
                           "word is not 8 characters long). This is likely an objcopy bug. "
                           "Continuing anyway...");
            }
            
            // The data word this token represents
            word_t data_word = (uint32_t)std::stoul(token, nullptr, 16);

            // Write the data word to memory and increment the address to the next word
            access_status_t access_status;
            write_memory(addr, DT_WORD, data_word, access_status);
            if(access_status != AS_OKAY) {
                return IL_FAIL;
            }
            addr += 4;
        }
    }
    return IL_OKAY;
}
