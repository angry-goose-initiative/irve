/**
 * @brief   Handles the memory of the emulator
 * 
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  Copyright (C) 2023 Nick Chan\n
 *  See the LICENSE file at the root of the project for licensing info.
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
#include <cstdio>

#include "csr.h"
#include "common.h"
#include "memory_map.h"
#include "rvexception.h"
#include "fuzzish.h"
#include "uart.h"

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

#define DATA_SIGN_MASK  0b100

#define WORD_ADDR_MASK  (~(uint64_t)0b11)

#define MPP_M_MODE      0b11

//A RISC-V page size is 4 KiB (0x1000 bytes)
#define PAGESIZE        0x1000

//Current address translation scheme
//0 = Bare (no address translation)
//1 = SV32
#define satp_MODE       (m_CSR_ref.implicit_read(Csr::Address::SATP).bit(31).u)
//The physical page number field of the satp CSR
#define satp_PPN        ((uint64_t)m_CSR_ref.implicit_read(Csr::Address::SATP).bits(21, 0).u)

//Make eXecutable readable field of the mstatus CSR
#define mstatus_MXR     (m_CSR_ref.implicit_read(Csr::Address::MSTATUS).bit(19).u)
//permit Superisor User Memory access field of the mstatus CSR
#define mstatus_SUM     (m_CSR_ref.implicit_read(Csr::Address::MSTATUS).bit(18).u)
//Modify PriVilege field of the mstatus CSR
#define mstatus_MPRV    (m_CSR_ref.implicit_read(Csr::Address::MSTATUS).bit(17).u)
//Previous privilige mode field of the mstatus CSR
#define mstatus_MPP     (m_CSR_ref.implicit_read(Csr::Address::MSTATUS).bits(12, 11).u)

//The virtual page number (VPN) of a virtual address (va)
#define va_VPN(i)       ((uint64_t)va.bits(21 + (10 * i), 12 + (10 * i)).u)

//Full physical page number field of the page table entry
#define pte_PPN         ((uint64_t)pte.bits(31, 10).u)
//Upper part of the page number field of the page table entry
#define pte_PPN1        ((uint64_t)pte.bits(31, 20).u)
//Lower part of the page number field of the page table entry
#define pte_PPN0        (pte.bits(19, 10).u)
#define pte_D           (pte.bit(7).u)    //Page dirty bit
#define pte_A           (pte.bit(6).u)    //Page accessed bit
#define pte_G           (pte.bit(5).u)    //Global mapping bit
#define pte_U           (pte.bit(4).u)    //Page is accessible in U-mode bit
#define pte_X           (pte.bit(3).u)    //Page is executable bit
#define pte_W           (pte.bit(2).u)    //Page is writable bit
#define pte_R           (pte.bit(1).u)    //Page is readable bit
#define pte_V           (pte.bit(0).u)    //Page table entry valid bit

//The current privilege mode
#define CURR_PMODE      m_CSR_ref.get_privilege_mode()

//True if a virtual memory access is not allowed
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
    (((CURR_PMODE == PrivilegeMode::SUPERVISOR_MODE) ||                                   \
        ((access_type != AT_INSTRUCTION) && (mstatus_MPP == 0b01) && (mstatus_MPRV == 1))) &&     \
        (mstatus_SUM == 0) && (pte_U == 1))

//Access types
#define AT_INSTRUCTION  0   //Access type is an instruction fetch
#define AT_LOAD         1   //Access type is a load
#define AT_STORE        3   //Access type is a store

#define PAGE_FAULT_BASE 12

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

Memory::Memory(Csr& CSR_ref):
        m_CSR_ref(CSR_ref),
        m_user_ram(new uint8_t[MEM_MAP_REGION_SIZE_USER_RAM]),
        m_kernel_ram(new uint8_t[MEM_MAP_REGION_SIZE_KERNEL_RAM]),
        m_aclint(CSR_ref),
        m_uart(),
        m_output_line_buffer() {

    //Check endianness of host (only little-endian hosts are supported)
    const union {uint8_t bytes[4]; uint32_t value;} host_order = {{0, 1, 2, 3}};
    assert((host_order.value == 0x03020100) && "Host endianness not supported");

    //Initialize all ram to random values
    irve_fuzzish_meminit(this->m_user_ram.get(), MEM_MAP_REGION_SIZE_USER_RAM);
    irve_fuzzish_meminit(this->m_kernel_ram.get(), MEM_MAP_REGION_SIZE_KERNEL_RAM);

    irvelog(1, "Created new Memory instance");
}

Memory::Memory(int imagec, const char* const* imagev, Csr& CSR_ref):
    m_CSR_ref(CSR_ref),
    m_user_ram(new uint8_t[MEM_MAP_REGION_SIZE_USER_RAM]),
    m_kernel_ram(new uint8_t[MEM_MAP_REGION_SIZE_KERNEL_RAM]),
    m_aclint(CSR_ref),
    m_uart(),
    m_output_line_buffer()
{

    //Check endianness of host (only little-endian hosts are supported)
    const union {uint8_t bytes[4]; uint32_t value;} host_order = {{0, 1, 2, 3}};
    assert((host_order.value == 0x03020100) && "Host endianness not supported");

    //Initialize all ram to random values
    irve_fuzzish_meminit(this->m_user_ram.get(), MEM_MAP_REGION_SIZE_USER_RAM);
    irve_fuzzish_meminit(this->m_kernel_ram.get(), MEM_MAP_REGION_SIZE_KERNEL_RAM);

    //Load memory images and throw an exception if an error occured
    image_load_status_t load_status;
    load_status = load_memory_image_files(imagec, imagev);
    if (load_status == IL_FAIL) {
        throw std::exception();
    }

    irvelog(1, "Created new Memory instance");
}

Memory::~Memory() {
    if (this->m_output_line_buffer.size() > 0) {
        irvelog_always_stdout(
            0,
            "\x1b[92mRVDEBUGADDR:\x1b[0m: \"\x1b[1m%s\x1b[0m\"",
            this->m_output_line_buffer.c_str()
        );
    }
}

Word Memory::instruction(Word addr) {
    access_status_t access_status;
    uint64_t machine_addr = translate_address(addr, AT_INSTRUCTION);

    Word data = read_memory(machine_addr, DT_WORD, access_status);

    if ((access_status == AS_VIOLATES_PMA) || (access_status == AS_VIOLATES_PMP)) {
        invoke_rv_exception(INSTRUCTION_ACCESS_FAULT);
    }

    if (access_status == AS_MISALIGNED) {
        invoke_rv_exception(INSTRUCTION_ADDRESS_MISALIGNED);
    }

    return data;
}

Word Memory::load(Word addr, uint8_t data_type) {
    assert((data_type <= 0b111) && "Invalid funct3");
    assert((data_type != 0b110) && "Invalid funct3");

    access_status_t access_status;
    uint64_t machine_addr = translate_address(addr, AT_LOAD);

    Word data = read_memory(machine_addr, data_type, access_status);

    if ((access_status == AS_VIOLATES_PMA) || (access_status == AS_VIOLATES_PMP)) {
        invoke_rv_exception(LOAD_ACCESS_FAULT);
    }

    if(access_status == AS_MISALIGNED) {
        invoke_rv_exception(LOAD_ADDRESS_MISALIGNED);
    }

    return data;
}

void Memory::store(Word addr, uint8_t data_type, Word data) {
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

uint64_t Memory::translate_address(Word untranslated_addr, uint8_t access_type) {
    if(no_address_translation(access_type)) {
        irvelog(1, "No address translation");
        return (uint64_t)untranslated_addr.u;
    }
    irvelog(1, "Translating address");

    access_status_t access_status;

    //The untranslated address is a virtual address
    Word va = untranslated_addr;

    //The address of a pte:
    // 33       12 11          2 1  0
    //|    PPN    |  va_VPN[i]  | 00 |
    //where i is the level of the page table

    //STEP 1
    //a is the PPN left shifted to its position in the pte
    uint64_t a = satp_PPN * PAGESIZE;
    uint64_t pte_addr;
    Word pte;

    irvelog(2, "Virtual address is 0x%08X", va.u);

    int i = 1;
    while(1) {
        // STEP 2
        pte_addr = a + (va_VPN(i) * 4);
        irvelog(2, "Accessing level %d pte at level at address 0x%09X", i, pte_addr);
        pte = read_memory(pte_addr, DT_WORD, access_status);
        if(access_status != AS_OKAY) {
            irvelog(2, "Accessing the pte violated a PMA or PMP check,"
                        "raising an access fault exception");
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

        //TODO what to do with the global bit?
        //assert(pte_G == 0 && "Global bit was set by software (but we haven't implemented it)");

        //STEP 3
        if(pte_V == 0 || (pte_R == 0 && pte_W == 1)) {
            irvelog(2, "The pte is not valid or the page is writable and"
                        "not readable, raising exception");
            invoke_rv_exception_by_num((rvexception::cause_t)(PAGE_FAULT_BASE + access_type));
        }

        //STEP 4
        if(pte_R == 1 || pte_X == 1) {
            irvelog(2, "Leaf pte found");
            break;
        }
        else {
            a = pte_PPN * PAGESIZE;
            --i;
            if(i < 0) {
                irvelog(2, "Leaf pte not found at the second level of the"
                            "page table, raising exception");
                invoke_rv_exception_by_num((rvexception::cause_t)(PAGE_FAULT_BASE + access_type));
            }
        }
    }

    //STEP 5
    if(ACCESS_NOT_ALLOWED) {
        irvelog(2, "This access is not allowed, raising exception");
        invoke_rv_exception_by_num((rvexception::cause_t)(PAGE_FAULT_BASE + access_type));
    }

    //STEP 6
    if((i == 1) && (pte_PPN0 != 0)) {
        //Misaligned superpage
        irvelog(2, "Misaligned superpage, raising exception");
        invoke_rv_exception_by_num((rvexception::cause_t)(PAGE_FAULT_BASE + access_type));
    }

    //STEP 7
    if((pte_A == 0) || ((access_type == AT_STORE) && (pte_D == 0))) {
        irvelog(2, "Accessed bit not set or operation is a store and the"
                    "dirty bit is not set, raising exception");
        invoke_rv_exception_by_num((rvexception::cause_t)(PAGE_FAULT_BASE + access_type));
    }

    //STEP 8
    uint64_t machine_addr = untranslated_addr.bits(11, 0).u;
    if(i == 1) {
        //Superpage translation
        machine_addr |= va_VPN(0) << 12;
        machine_addr |= pte_PPN1 << 22;
    }
    else {
        machine_addr |= pte_PPN << 12;
    }
    irvelog(2, "Translation resulted in address 0x%09X", machine_addr);

    return machine_addr;
}

bool Memory::no_address_translation(uint8_t access_type) const {
    if(mstatus_MPRV && (access_type != AT_INSTRUCTION)) {
        //The modify privilege mode flag is set and the access type is not instruction
        if(mstatus_MPP == MPP_M_MODE || (satp_MODE == 0)) {
            return true;
        }
        else {
            return false;
        }
    }
    else {
        if((CURR_PMODE == PrivilegeMode::MACHINE_MODE) || (satp_MODE == 0)) {
            return true;
        }
        else {
            return false;
        }
    }
    assert(false && "Should never get here");
    return true;
}

Word Memory::read_memory(
        uint64_t addr, uint8_t data_type, access_status_t& access_status) {

    assert(((addr & 0xFFFFFFFC00000000) == 0) && "Address should only be 34 bits!");

    irvelog(2, "Reading from machine address 0x%09X", addr);

    access_status = AS_OKAY; //Set here to avoid uninitialized warning

    Word data = 0;

    //All regions that contain readable memory should be covered
    if (addr <= MEM_MAP_REGION_END_USER_RAM) {
        data = read_memory_region_user_ram(addr, data_type, access_status);
    }
    else if ((addr >= MEM_MAP_REGION_START_KERNEL_RAM) && (addr <= MEM_MAP_REGION_END_KERNEL_RAM)) {
        data = read_memory_region_kernel_ram(addr, data_type, access_status);
    }
    else if ((addr >= MEM_MAP_REGION_START_ACLINT) && (addr <= MEM_MAP_REGION_END_ACLINT)) {
        data = read_memory_region_aclint(addr, data_type, access_status);
    }
    else if ((addr >= MEM_MAP_REGION_START_UART) && (addr <= MEM_MAP_REGION_END_UART)) {
        data = read_memory_region_uart(addr, data_type, access_status);
    }
    else {
        access_status = AS_VIOLATES_PMA;
    }
    
    if (access_status != AS_OKAY) {
        return Word(0);
    }

    return data;

}

Word Memory::read_memory_region_user_ram(
        uint64_t addr, uint8_t data_type, access_status_t& access_status) const {

    assert((addr <= MEM_MAP_REGION_END_USER_RAM) && "This should never happen");

    //Check for misaligned access
    if (((data_type & DATA_WIDTH_MASK) == DT_HALFWORD) && ((addr & 0b1) != 0)) {
        //Misaligned halfword read
        access_status = AS_MISALIGNED;
        return Word(0);
    }
    else if ((data_type == DT_WORD) && ((addr & 0b11) != 0)) {
        //Misaligned word read
        access_status = AS_MISALIGNED;
        return Word(0);
    }

    Word data;
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

Word Memory::read_memory_region_kernel_ram(
        uint64_t addr, uint8_t data_type, access_status_t& access_status) const {

    assert((addr >= MEM_MAP_REGION_START_KERNEL_RAM) && (addr <= MEM_MAP_REGION_END_KERNEL_RAM) &&
            "This should never happen");
    
    //Check for misaligned access
    if (((data_type & DATA_WIDTH_MASK) == DT_HALFWORD) && ((addr & 0b1) != 0)) {
        //Misaligned halfword read
        access_status = AS_MISALIGNED;
        return Word(0);
    }
    else if ((data_type == DT_WORD) && ((addr & 0b11) != 0)) {
        //Misaligned word read
        access_status = AS_MISALIGNED;
        return Word(0);
    }

    Word data;
    uint64_t mem_index = addr - MEM_MAP_REGION_START_KERNEL_RAM;
    void* mem_ptr = &(m_kernel_ram[mem_index]);
    switch (data_type) {
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

Word Memory::read_memory_region_aclint(
        uint64_t addr, uint8_t data_type, access_status_t& access_status) {

    assert((addr >= MEM_MAP_REGION_START_ACLINT) && (addr <= MEM_MAP_REGION_END_ACLINT) &&
            "This should never happen");

    //These registers must be accessed as words only
    if (data_type != DT_WORD) {
        access_status = AS_VIOLATES_PMA;
        return Word(0);
    }

    //Check for misaligned word access
    if ((addr & 0b11) != 0) {
        //Misaligned word
        access_status = AS_MISALIGNED;
        return Word(0);
    }

    return this->m_aclint.read(static_cast<Aclint::Address>(addr - MEM_MAP_REGION_START_ACLINT));
}

Word Memory::read_memory_region_uart(
        uint64_t addr, uint8_t data_type, access_status_t& access_status) {

    assert((addr >= MEM_MAP_REGION_START_UART) && (addr <= MEM_MAP_REGION_END_UART) &&
            "This should never happen");

    assert(false && "Region not implemented yet");

    //Only byte accesses allowed
    if ((data_type & DATA_WIDTH_MASK) != DT_BYTE) {
        access_status = AS_VIOLATES_PMA;
        return Word(0);
    }

    uint8_t uart_addr = (uint8_t)(addr - MEM_MAP_REGION_START_UART);

    Word data;
    
    //TODO uart read should also update access_status?
    if (data_type & DATA_SIGN_MASK) {
        data.u = (uint32_t)this->m_uart.read(uart_addr);
    }
    else {
        data.s = (int32_t)this->m_uart.read(uart_addr);
    }
    
    return data;
}

void Memory::write_memory(uint64_t addr, uint8_t data_type, Word data,
                                    access_status_t &access_status) {

    assert(((addr & 0xFFFFFFFC00000000) == 0) && "Address should only be 34 bits!");
    
    irvelog(2, "Writing to machine address 0x%09X", addr);

    access_status = AS_OKAY; //Set here to avoid uninitialized warning

    //All regions that contain writable memory should be covered
    if (addr <= MEM_MAP_REGION_END_USER_RAM) {
        write_memory_region_user_ram(addr, data_type, data, access_status);
    }
    else if ((addr >= MEM_MAP_REGION_START_KERNEL_RAM) && (addr <= MEM_MAP_REGION_END_KERNEL_RAM)) {
        write_memory_region_kernel_ram(addr, data_type, data, access_status);
    }
    else if ((addr >= MEM_MAP_REGION_START_ACLINT) && (addr <= MEM_MAP_REGION_END_ACLINT)) {
        write_memory_region_aclint(addr, data_type, data, access_status);
    }
    else if ((addr >= MEM_MAP_REGION_START_UART) && (addr <= MEM_MAP_REGION_END_UART)) {
        write_memory_region_uart(addr, data_type, data, access_status);
    }
    else if (addr == MEM_MAP_ADDR_DEBUG) {
        write_memory_region_debug(addr, data_type, data, access_status);
    }
    else {
        access_status = AS_VIOLATES_PMA;
    }

}

void Memory::write_memory_region_user_ram(uint64_t addr, uint8_t data_type, Word data,
                                                    access_status_t& access_status) {

    assert((addr <= MEM_MAP_REGION_END_USER_RAM) && "This should never happen");

    //Check for misaligned access
    if (((data_type & DATA_WIDTH_MASK) == DT_HALFWORD) && ((addr & 0b1) != 0)) {
        //Misaligned halfword write
        access_status = AS_MISALIGNED;
    }
    else if ((data_type == DT_WORD) && ((addr & 0b11) != 0)) {
        //Misaligned word write
        access_status = AS_MISALIGNED;
    }

    uint64_t mem_index = addr - MEM_MAP_REGION_START_USER_RAM;
    void* mem_ptr = &(m_user_ram[mem_index]);
    switch (data_type) {
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

void Memory::write_memory_region_kernel_ram(uint64_t addr, uint8_t data_type, Word data,
                                                        access_status_t& access_status) {

    assert(((addr >= MEM_MAP_REGION_START_KERNEL_RAM) && (addr <= MEM_MAP_REGION_END_KERNEL_RAM))
            && "This should never happen");

    //Check for misaligned access
    if (((data_type & DATA_WIDTH_MASK) == DT_HALFWORD) && ((addr & 0b1) != 0)) {
        // Misaligned halfword write
        access_status = AS_MISALIGNED;
    }
    else if ((data_type == DT_WORD) && ((addr & 0b11) != 0)) {
        //Misaligned word write
        access_status = AS_MISALIGNED;
    }

    uint64_t mem_index = addr - MEM_MAP_REGION_START_KERNEL_RAM;
    void* mem_ptr = &(m_kernel_ram[mem_index]);
    switch (data_type) {
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
    
void Memory::write_memory_region_aclint(uint64_t addr, uint8_t data_type, Word data,
                                                    access_status_t& access_status) {
    assert(((addr >= MEM_MAP_REGION_START_ACLINT) && (addr <= MEM_MAP_REGION_END_ACLINT))
            && "This should never happen");

    //These registers must be accessed as words only
    if (data_type != DT_WORD) {
        access_status = AS_VIOLATES_PMA;
        return;
    }

    //Check for misaligned access. We only check for a misaligned word since at this point, the
    //data width has to be a word.
    if ((addr & 0b11) != 0) {
        //Misaligned word
        access_status = AS_MISALIGNED;
        return;
    }

    this->m_aclint.write(static_cast<Aclint::Address>(addr - MEM_MAP_REGION_START_ACLINT), data);
}

void Memory::write_memory_region_uart(uint64_t addr, uint8_t data_type, Word data,
                                                access_status_t& access_status) {

    assert((addr >= MEM_MAP_REGION_START_UART) && (addr <= MEM_MAP_REGION_END_UART) &&
            "This should never happen");

    assert(false && "Region not implemented yet");

    //Only byte accesses allowed
    if ((data_type & DATA_WIDTH_MASK) != DT_BYTE) {
        access_status = AS_VIOLATES_PMA;
        return;
    }

    uint8_t uart_addr = (uint8_t)(addr - MEM_MAP_REGION_START_UART);
    uint8_t uart_data = (uint8_t)data.u;

    //TODO uart write can update access_status?
    this->m_uart.write(uart_addr, uart_data);
}

void Memory::write_memory_region_debug(uint64_t addr, uint8_t data_type, Word data,
                                                    access_status_t& access_status) {
            
    assert((addr == MEM_MAP_ADDR_DEBUG) && "This should never happen");

    //This region can only be written to with a byte access
    if (data_type != DT_BYTE) {
        access_status = AS_VIOLATES_PMA;
        return;
    }

    char character = (char)data.s;
    switch (character) {
        case '\n':
            //End of line; print the contents of the line buffer and clear it
            irvelog_always_stdout(
                0,
                "\x1b[92mRVDEBUGADDR\x1b[0m: \"\x1b[1m%s\x1b[0m\\n\"",
                this->m_output_line_buffer.c_str()
            );
            this->m_output_line_buffer.clear();
            break;
        case '\0':
            //Null terminator; print the contents of the line buffer and clear it
            //(this has helped with debugging weird issues in the past)
            irvelog_always_stdout(
                0,
                "\x1b[92mRVDEBUGADDR\x1b[0m: \"\x1b[1m%s\x1b[0m\\0\"",
                this->m_output_line_buffer.c_str()
            );
            this->m_output_line_buffer.clear();
            break;
        case '\r':  this->m_output_line_buffer += "\x1b[0m\\r\x1b[1m"; break;//Print \r in non-bold
        default:    this->m_output_line_buffer.push_back(character); break;
    }
}

image_load_status_t Memory::load_memory_image_files(
        int imagec, const char* const* imagev) {

    // Load each memory file
    for (int i = 0; i < imagec; ++i) {
        std::string path = imagev[i];
        if (path.find("/") == std::string::npos) {
            path = TESTFILES_DIR + path;
        }
        irvelog_always(0, "Loading memory image from file \"%s\"", path.c_str());
        image_load_status_t load_status;
        if (path.find(".vhex8") != std::string::npos) {
            load_status = this->load_verilog_8(path);
        } else if (path.find(".vhex32") != std::string::npos) {
            load_status = this->load_verilog_32(path);
        } else {//Assume it's a raw binary file
            //TODO: Make this configurable (this is a sensible default since the Linux kernel
            //produces a raw `Image` file)
            load_status = this->load_raw_bin(path, 0xC0000000);
        }
        if(load_status == IL_FAIL) {
            return IL_FAIL;
        }
    }
    return IL_OKAY;
}

image_load_status_t Memory::load_raw_bin(std::string image_path,
                                                            uint64_t start_addr) {
    //Open the file
    const char* filename = image_path.c_str();
    FILE* file = fopen(filename, "rb");
    if (!file) {
        irvelog(1, "Failed to open memory image file \"%s\"", filename);
        return IL_FAIL;
    }

    //Get the file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);
    if (file_size < 0) {
        irvelog(1, "Failed to get file size");
        fclose(file);
        return IL_FAIL;
    }
    irvelog(1, "Memory image file size is %ld bytes", file_size);

    //Read a file into the emulator byte-by-byte
    //TODO do this more efficiently with fread()
    for (long i = 0; i < file_size; ++i) {
        Word data_byte = fgetc(file);
        uint64_t addr = start_addr + (uint64_t)i;

        access_status_t access_status;
        write_memory(addr, DT_BYTE, data_byte, access_status);
        if (access_status != AS_OKAY) {
            fclose(file);
            return IL_FAIL;
        }
        ++addr;
    }

    //Close the file
    fclose(file);
    return IL_OKAY;
}

image_load_status_t Memory::load_verilog_8(std::string image_path) {
    std::fstream fin = std::fstream(image_path);
    if (!fin) {
        return IL_FAIL;
    }

    //Read the file token by token
    uint64_t addr = 0;
    std::string token;
    while (fin >> token) {
        assert((token.length() != 0) && "This should never happen");
        if (token.at(0) == '@') { //`@` indicates a new address (ASSUMING 32-BIT WORDS)
            std::string new_addr_str = token.substr(1);
            assert((new_addr_str.length() == 8) &&
                    "Memory image file is not formatted correctly (bad address)");
            addr = std::stoul(new_addr_str, nullptr, 16);
        }
        else { //New data word (32-bit, could be an instruction or data)
            if (token.length() != 2) {
                irvelog(1, "Memory image file is not formatted correctly (bad data)");
                return IL_FAIL;
            }
            
            //The data word this token represents
            Word data_word = (uint32_t)std::stoul(token, nullptr, 16);

            //Write the data word to memory and increment the address to the next word
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

image_load_status_t Memory::load_verilog_32(std::string image_path) {
    std::fstream fin = std::fstream(image_path);
    if (!fin) {
        return IL_FAIL;
    }

    //Read the file token by token
    uint64_t addr = 0;
    std::string token;
    while (fin >> token) {
        assert((token.length() != 0) && "This should never happen");
        if (token.at(0) == '@') { //`@` indicates a new address (ASSUMING 32-BIT WORDS)
            std::string new_addr_str = token.substr(1);
            assert((new_addr_str.length() == 8) &&
                    "Memory image file is not formatted correctly (bad address)");
            addr = std::stoul(new_addr_str, nullptr, 16);
            addr *= 4; //This is a word address, not a byte address, so multiply by 4
        }
        else { //New data word (32-bit, could be an instruction or data)
            if (token.length() != 8) {
                irvelog(1, "Warning: 32-bit Verilog image file is not formatted correctly (data "
                           "word is not 8 characters long). This is likely an objcopy bug. "
                           "Continuing anyway with assumed leading zeroes...");
            }
            
            //The data word this token represents
            Word data_word = (uint32_t)std::stoul(token, nullptr, 16);

            //Write the data word to memory and increment the address to the next word
            access_status_t access_status;
            write_memory(addr, DT_WORD, data_word, access_status);
            if (access_status != AS_OKAY) {
                return IL_FAIL;
            }
            addr += 4;
        }
    }
    return IL_OKAY;
}
