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
#include <memory>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cstdio>

#include "csr.h"
#include "common.h"
#include "memory_map.h"
#include "rv_trap.h"
#include "fuzzish.h"
#include "uart.h"

#define INST_COUNT 0 // We only log at init
#include "logging.h"

using namespace irve::internal;

/* ------------------------------------------------------------------------------------------------
 * Constants/Defines
 * --------------------------------------------------------------------------------------------- */

// The directory where test files are located
// FIXME this should be moved to config.h
#define TESTFILES_DIR   "rvsw/compiled/"

constexpr uint8_t DATA_WIDTH_MASK   {0b11};

constexpr uint8_t DATA_SIGN_MASK    {0b100};

// A RISC-V page size is 4 KiB (0x1000 bytes)
constexpr uint64_t PAGESIZE         {0x1000};

//Current address translation scheme
//0 = Bare (no address translation)
//1 = SV32
#define satp_MODE       (csr.implicit_read(Csr::Address::SATP).bit(31).u)
//The physical page number field of the satp CSR
#define satp_PPN        ((uint64_t)csr.implicit_read(Csr::Address::SATP).bits(21, 0).u)

//Make eXecutable readable field of the mstatus CSR
#define mstatus_MXR     (csr.implicit_read(Csr::Address::MSTATUS).bit(19).u)
//permit Superisor User Memory access field of the mstatus CSR
#define mstatus_SUM     (csr.implicit_read(Csr::Address::MSTATUS).bit(18).u)
//Modify PriVilege field of the mstatus CSR
#define mstatus_MPRV    (csr.implicit_read(Csr::Address::MSTATUS).bit(17).u)
//Previous privilige mode field of the mstatus CSR
#define mstatus_MPP     (csr.implicit_read(Csr::Address::MSTATUS).bits(12, 11).u)

//The virtual page number (VPN) of a virtual address (va)
#define va_VPN(i)       ((uint64_t)va.bits(21 + (10 * i), 12 + (10 * i)).u)

//True if a virtual memory access is not allowed
#define ACCESS_NOT_ALLOWED                                                                        \
/* The page cannot be accessed if any of the following conditions is met: */                      \
    /* 1. Fetching an instruction but the page is not marked as executable */                     \
    ((access_type == AccessType::INSTRUCTION) && (pte.X() != 1)) ||                                          \
    /* 2. Access is a store but the page is not marked as writable */                             \
    ((access_type == AccessType::STORE) && (pte.W() != 1)) ||                                                \
    /* 3. Access is a load but the page is either not marked as readable or mstatus.MXR is set */ \
    /*    but the page isn't marked as executable */                                              \
    ((access_type == AccessType::LOAD) && ((pte.R() != 1) && ((mstatus_MXR == 0) || (pte.X() == 0)))) ||       \
    /* 4. Either the current privilege mode is S or the effective privilege mode is S with the */ \
    /*    access being a load or a store (not an instruction) and S-mode can't access U-mode */   \
    /*    pages and the page is markes as accessible in U-mode */                                 \
    (((csr.get_privilege_mode() == PrivilegeMode::SUPERVISOR_MODE) ||                                   \
        ((access_type != AccessType::INSTRUCTION) && (mstatus_MPP == 0b01) && (mstatus_MPRV == 1))) &&     \
        (mstatus_SUM == 0) && (pte.U() == 1))

constexpr uint32_t PAGE_FAULT_BASE{12};

// Makes accessing page table entry fields easier
class Pte {
public:
    Pte() = default;
    inline void operator=(Word word) { this->pte = word; }
    
    inline uint64_t PPN() { return this->pte.bits(31, 10).u; } // Physical page number
    inline uint64_t PPN1() { return this->pte.bits(31, 20).u; }
    inline uint64_t PPN0() { return this->pte.bits(19, 10).u; }
    inline bool D() { return this->pte.bit(7).u; } // Page dirty bit
    inline bool A() { return this->pte.bit(6).u; } // Page accessed bit
    inline bool G() { return this->pte.bit(5).u; } // Global mapping bit
    inline bool U() { return this->pte.bit(4).u; } // Page is accessible in U-mode bit
    inline bool X() { return this->pte.bit(3).u; } // Page is executable bit
    inline bool W() { return this->pte.bit(2).u; } // Page is writable bit
    inline bool R() { return this->pte.bit(1).u; } // Page is readable bit
    inline bool V() { return this->pte.bit(0).u; } // Page table entry valid bit
private:
    Word pte;
};

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

Memory::Memory(Csr& CSR_ref, int imagec, const char* const* imagev):
    csr(CSR_ref),
    m_user_ram(new uint8_t[mmap::USER_RAM.SIZE]),
    m_kernel_ram(new uint8_t[mmap::KERNEL_RAM.SIZE]),
    m_aclint(CSR_ref),
    m_uart(),
    m_output_line_buffer()
{

    //Check endianness of host (only little-endian hosts are supported)
    const union {uint8_t bytes[4]; uint32_t value;} host_order = {{0, 1, 2, 3}};
    assert((host_order.value == 0x03020100) && "Host endianness not supported");

    //Initialize all ram to random values
    fuzzish::meminit(this->m_user_ram.get(), mmap::USER_RAM.SIZE);
    fuzzish::meminit(this->m_kernel_ram.get(), mmap::KERNEL_RAM.SIZE);

    //Load memory images and throw an exception if an error occured
    ImageLoadStatus load_status;
    load_status = load_memory_image_files(imagec, imagev);
    if (load_status == ImageLoadStatus::ERROR) {
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
    AccessStatus access_status;
    uint64_t machine_addr = translate_address(addr, AccessType::INSTRUCTION);

    Word data = read_memory(machine_addr, DT_WORD, access_status);

    if ((access_status == AccessStatus::VIOLATES_PMA) || (access_status == AccessStatus::VIOLATES_PMP)) {
        rv_trap::invoke_exception(rv_trap::Cause::INSTRUCTION_ACCESS_FAULT_EXCEPTION);
    }

    if (access_status == AccessStatus::MISALIGNED) {
        rv_trap::invoke_exception(rv_trap::Cause::INSTRUCTION_ADDRESS_MISALIGNED_EXCEPTION);
    }

    return data;
}

Word Memory::load(Word addr, uint8_t data_type) {
    assert((data_type <= 0b111) && "Invalid funct3");
    assert((data_type != 0b110) && "Invalid funct3");

    AccessStatus access_status;
    uint64_t machine_addr = translate_address(addr, AccessType::LOAD);

    Word data = read_memory(machine_addr, data_type, access_status);

    if ((access_status == AccessStatus::VIOLATES_PMA) || (access_status == AccessStatus::VIOLATES_PMP)) {
        rv_trap::invoke_exception(rv_trap::Cause::LOAD_ACCESS_FAULT_EXCEPTION);
    }

    if(access_status == AccessStatus::MISALIGNED) {
        rv_trap::invoke_exception(rv_trap::Cause::LOAD_ADDRESS_MISALIGNED_EXCEPTION);
    }

    return data;
}

void Memory::store(Word addr, uint8_t data_type, Word data) {
    assert((data_type <= 0b010) && "Invalid funct3");

    AccessStatus access_status;
    uint64_t machine_addr = translate_address(addr, AccessType::STORE);

    write_memory(machine_addr, data_type, data, access_status);

    if((access_status == AccessStatus::VIOLATES_PMA) || (access_status == AccessStatus::VIOLATES_PMP)) {
        rv_trap::invoke_exception(rv_trap::Cause::STORE_OR_AMO_ACCESS_FAULT_EXCEPTION);
    }

    if(access_status == AccessStatus::MISALIGNED) {
        rv_trap::invoke_exception(rv_trap::Cause::STORE_OR_AMO_ADDRESS_MISALIGNED_EXCEPTION);
    }
}

uint64_t Memory::translate_address(Word untranslated_addr, AccessType access_type) {
    if(no_address_translation(access_type)) {
        irvelog(1, "No address translation");
        return (uint64_t)untranslated_addr.u;
    }
    irvelog(1, "Translating address");

    AccessStatus access_status;

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
    Pte pte;

    irvelog(2, "Virtual address is 0x%08X", va.u);

    int i = 1;
    while(1) {
        // STEP 2
        pte_addr = a + (va_VPN(i) * 4);
        irvelog(2, "Accessing level %d pte at level at address 0x%09X", i, pte_addr);
        pte = read_memory(pte_addr, DT_WORD, access_status);
        if (access_status != AccessStatus::OKAY) {
            irvelog(2, "Accessing the pte violated a PMA or PMP check,"
                        "raising an access fault exception");
            switch(access_type) {
                case AccessType::INSTRUCTION:
                    rv_trap::invoke_exception(rv_trap::Cause::INSTRUCTION_ACCESS_FAULT_EXCEPTION);
                    break;
                case AccessType::LOAD:
                    rv_trap::invoke_exception(rv_trap::Cause::LOAD_ACCESS_FAULT_EXCEPTION);
                    break;
                case AccessType::STORE:
                    rv_trap::invoke_exception(rv_trap::Cause::STORE_OR_AMO_ACCESS_FAULT_EXCEPTION);
                    break;
                default:
                    assert(false && "Should never get here");
            }
        }
        irvelog(2, "pte found = 0x%08X", pte.u);

        //TODO what to do with the global bit?
        //assert(pte_G == 0 && "Global bit was set by software (but we haven't implemented it)");

        //STEP 3
        if (pte.V() == 0 || (pte.R() == 0 && pte.W() == 1)) {
            irvelog(2, "The pte is not valid or the page is writable and"
                        "not readable, raising exception");
            rv_trap::invoke_exception(static_cast<rv_trap::Cause>(PAGE_FAULT_BASE + static_cast<uint32_t>(access_type)));
        }

        //STEP 4
        if (pte.R() == 1 || pte.X() == 1) {
            irvelog(2, "Leaf pte found");
            break;
        }
        else {
            a = pte.PPN() * PAGESIZE;
            --i;
            if(i < 0) {
                irvelog(2, "Leaf pte not found at the second level of the"
                            "page table, raising exception");
                rv_trap::invoke_exception(static_cast<rv_trap::Cause>(PAGE_FAULT_BASE + static_cast<uint32_t>(access_type)));
            }
        }
    }

    //STEP 5
    if(ACCESS_NOT_ALLOWED) {
        irvelog(2, "This access is not allowed, raising exception");
        rv_trap::invoke_exception(static_cast<rv_trap::Cause>(PAGE_FAULT_BASE + static_cast<uint32_t>(access_type)));
    }

    //STEP 6
    if((i == 1) && (pte.PPN0() != 0)) {
        //Misaligned superpage
        irvelog(2, "Misaligned superpage, raising exception");
        rv_trap::invoke_exception(static_cast<rv_trap::Cause>(PAGE_FAULT_BASE + static_cast<uint32_t>(access_type)));
    }

    //STEP 7
    if ((pte.A() == 0) || ((access_type == AccessType::STORE) && (pte.D() == 0))) {
        irvelog(2, "Accessed bit not set or operation is a store and the"
                    "dirty bit is not set, raising exception");
        rv_trap::invoke_exception(static_cast<rv_trap::Cause>(PAGE_FAULT_BASE + static_cast<uint32_t>(access_type)));
    }

    //STEP 8
    uint64_t machine_addr = untranslated_addr.bits(11, 0).u;
    if (i == 1) {
        //Superpage translation
        machine_addr |= va_VPN(0) << 12;
        machine_addr |= pte.PPN1() << 22;
    }
    else {
        machine_addr |= pte.PPN() << 12;
    }
    irvelog(2, "Translation resulted in address 0x%09X", machine_addr);

    return machine_addr;
}

bool Memory::no_address_translation(AccessType access_type) const {
    if (mstatus_MPRV && (access_type != AccessType::INSTRUCTION)) {
        //The modify privilege mode flag is set and the access type is not instruction
        if (static_cast<PrivilegeMode>(mstatus_MPP) == PrivilegeMode::MACHINE_MODE || (satp_MODE == 0)) {
            return true;
        }
        else {
            return false;
        }
    }
    else {
        if((csr.get_privilege_mode() == PrivilegeMode::MACHINE_MODE) || (satp_MODE == 0)) {
            return true;
        }
        else {
            return false;
        }
    }
    assert(false && "Should never get here");
    return true;
}

Word Memory::read_memory(const uint64_t addr, const uint8_t data_type,
                         AccessStatus& access_status)
{

    assert(((addr & 0xFFFFFFFC00000000) == 0) && "Address should only be 34 bits!");

    irvelog(2, "Reading from machine address 0x%09X", addr);

    access_status = AccessStatus::OKAY; //Set here to avoid uninitialized warning

    Word data{};

    const auto addr34{static_cast<mmap::Addr34>(addr)};

    //All regions that contain readable memory should be covered
    if (mmap::USER_RAM.in_region(addr34)) {
        data = read_memory_region_user_ram(addr, data_type, access_status);
    }
    else if (mmap::KERNEL_RAM.in_region(addr34)) {
        data = read_memory_region_kernel_ram(addr, data_type, access_status);
    }
    else if (mmap::ACLINT.in_region(addr34)) {
        data = read_memory_region_aclint(addr, data_type, access_status);
    }
    else if (mmap::UART.in_region(addr34)) {
        data = read_memory_region_uart(addr, data_type, access_status);
    }
    else {
        access_status = AccessStatus::VIOLATES_PMA;
    }
    
    return data;
}

Word Memory::read_memory_region_user_ram(uint64_t addr, uint8_t data_type,
                                         AccessStatus& access_status) const
{
    //Check for misaligned access
    if (((data_type & DATA_WIDTH_MASK) == DT_HALFWORD) && ((addr & 0b1) != 0)) {
        //Misaligned halfword read
        access_status = AccessStatus::MISALIGNED;
        return Word(0);
    }
    else if ((data_type == DT_WORD) && ((addr & 0b11) != 0)) {
        //Misaligned word read
        access_status = AccessStatus::MISALIGNED;
        return Word(0);
    }

    Word data;
    uint64_t mem_index = addr - mmap::USER_RAM.START;
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

Word Memory::read_memory_region_kernel_ram(uint64_t addr, uint8_t data_type,
                                           AccessStatus& access_status) const 
{
    //Check for misaligned access
    if (((data_type & DATA_WIDTH_MASK) == DT_HALFWORD) && ((addr & 0b1) != 0)) {
        //Misaligned halfword read
        access_status = AccessStatus::MISALIGNED;
        return Word(0);
    }
    else if ((data_type == DT_WORD) && ((addr & 0b11) != 0)) {
        //Misaligned word read
        access_status = AccessStatus::MISALIGNED;
        return Word(0);
    }

    Word data;
    uint64_t mem_index = addr - mmap::KERNEL_RAM.START;
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

Word Memory::read_memory_region_aclint(uint64_t addr, uint8_t data_type,
                                       AccessStatus& access_status)
{
    //These registers must be accessed as words only
    if (data_type != DT_WORD) {
        access_status = AccessStatus::VIOLATES_PMA;
        return Word(0);
    }

    //Check for misaligned word access
    if ((addr & 0b11) != 0) {
        //Misaligned word
        access_status = AccessStatus::MISALIGNED;
        return Word(0);
    }

    return this->m_aclint.read(static_cast<Aclint::Address>(addr - mmap::ACLINT.START));
}

Word Memory::read_memory_region_uart(uint64_t addr, uint8_t data_type,
                                     AccessStatus& access_status)
{
    assert(false && "Region not implemented yet");

    //Only byte accesses allowed
    if ((data_type & DATA_WIDTH_MASK) != DT_BYTE) {
        access_status = AccessStatus::VIOLATES_PMA;
        return Word(0);
    }

    auto uart_addr = static_cast<Uart::Address>(addr - mmap::UART.START);

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
                          AccessStatus &access_status)
{
    assert(((addr & 0xFFFFFFFC00000000) == 0) && "Address should only be 34 bits!");
    
    irvelog(2, "Writing to machine address 0x%09X", addr);

    access_status = AccessStatus::OKAY; // Set here to avoid uninitialized warning

    auto addr34{static_cast<mmap::Addr34>(addr)};

    //All regions that contain writable memory should be covered
    if (mmap::USER_RAM.in_region(addr34)) {
        write_memory_region_user_ram(addr, data_type, data, access_status);
    }
    else if (mmap::KERNEL_RAM.in_region(addr34)) {
        write_memory_region_kernel_ram(addr, data_type, data, access_status);
    }
    else if (mmap::ACLINT.in_region(addr34)) {
        write_memory_region_aclint(addr, data_type, data, access_status);
    }
    else if (mmap::UART.in_region(addr34)) {
        write_memory_region_uart(addr, data_type, data, access_status);
    }
    else if (mmap::DEBUG.in_region(addr34)) {
        write_memory_region_debug(addr, data_type, data, access_status);
    }
    else {
        access_status = AccessStatus::VIOLATES_PMA;
    }

}

void Memory::write_memory_region_user_ram(uint64_t addr, uint8_t data_type, Word data,
                                          AccessStatus& access_status)
{
    //Check for misaligned access
    if (((data_type & DATA_WIDTH_MASK) == DT_HALFWORD) && ((addr & 0b1) != 0)) {
        //Misaligned halfword write
        access_status = AccessStatus::MISALIGNED;
    }
    else if ((data_type == DT_WORD) && ((addr & 0b11) != 0)) {
        //Misaligned word write
        access_status = AccessStatus::MISALIGNED;
    }

    uint64_t mem_index = addr - mmap::USER_RAM.START;
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
                                            AccessStatus& access_status)
{
    //Check for misaligned access
    if (((data_type & DATA_WIDTH_MASK) == DT_HALFWORD) && ((addr & 0b1) != 0)) {
        // Misaligned halfword write
        access_status = AccessStatus::MISALIGNED;
    }
    else if ((data_type == DT_WORD) && ((addr & 0b11) != 0)) {
        //Misaligned word write
        access_status = AccessStatus::MISALIGNED;
    }

    uint64_t mem_index = addr - mmap::KERNEL_RAM.START;
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
                                        AccessStatus& access_status)
{
    //These registers must be accessed as words only
    if (data_type != DT_WORD) {
        access_status = AccessStatus::VIOLATES_PMA;
        return;
    }

    //Check for misaligned access. We only check for a misaligned word since at this point, the
    //data width has to be a word.
    if ((addr & 0b11) != 0) {
        //Misaligned word
        access_status = AccessStatus::MISALIGNED;
        return;
    }

    this->m_aclint.write(static_cast<Aclint::Address>(addr - mmap::ACLINT.START), data);
}

void Memory::write_memory_region_uart(uint64_t addr, uint8_t data_type, Word data,
                                      AccessStatus& access_status)
{
    assert(false && "Region not implemented yet");

    // Only byte accesses allowed
    if ((data_type & DATA_WIDTH_MASK) != DT_BYTE) {
        access_status = AccessStatus::VIOLATES_PMA;
        return;
    }

    auto uart_addr = static_cast<Uart::Address>(addr - mmap::UART.START);
    uint8_t uart_data = (uint8_t)data.u;

    // TODO uart write can update access_status?
    this->m_uart.write(uart_addr, uart_data);
}

void Memory::write_memory_region_debug(uint64_t /*unused*/, uint8_t data_type, Word data,
                                       AccessStatus& access_status)
{
    // This region can only be written to with a byte access
    if (data_type != DT_BYTE) {
        access_status = AccessStatus::VIOLATES_PMA;
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

Memory::ImageLoadStatus Memory::load_memory_image_files(int imagec, const char* const* imagev) {

    // Load each memory file
    for (int i = 0; i < imagec; ++i) {
        std::string path = imagev[i];
        if (path.find("/") == std::string::npos) {
            path = TESTFILES_DIR + path;
        }
        irvelog_always(0, "Loading memory image from file \"%s\"", path.c_str());
        ImageLoadStatus load_status;
        if (path.find(".vhex8") != std::string::npos) {
            load_status = this->load_verilog_8(path);
        } else if (path.find(".vhex32") != std::string::npos) {
            load_status = this->load_verilog_32(path);
        } else {//Assume it's a raw binary file
            //TODO: Make this configurable (this is a sensible default since the Linux kernel
            //produces a raw `Image` file)
            load_status = this->load_raw_bin(path, 0xC0000000);
        }
        if (load_status == ImageLoadStatus::ERROR) {
            return ImageLoadStatus::ERROR;
        }
    }
    return ImageLoadStatus::OKAY;
}

Memory::ImageLoadStatus Memory::load_raw_bin(std::string image_path,
                                         uint64_t start_addr)
{
    // Open the file
    const char* filename = image_path.c_str();
    FILE* file = fopen(filename, "rb");
    if (!file) {
        irvelog(1, "Failed to open memory image file \"%s\"", filename);
        return ImageLoadStatus::ERROR;
    }

    //Get the file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);
    if (file_size < 0) {
        irvelog(1, "Failed to get file size");
        fclose(file);
        return ImageLoadStatus::ERROR;
    }
    irvelog(1, "Memory image file size is %ld bytes", file_size);

    // Read a file into the emulator byte-by-byte
    // TODO do this more efficiently with fread()
    for (long i = 0; i < file_size; ++i) {
        Word data_byte = fgetc(file);
        uint64_t addr = start_addr + (uint64_t)i;

        AccessStatus access_status;
        write_memory(addr, DT_BYTE, data_byte, access_status);
        if (access_status != AccessStatus::OKAY) {
            fclose(file);
            return ImageLoadStatus::ERROR;
        }
        ++addr;
    }

    // Close the file
    fclose(file);
    return ImageLoadStatus::OKAY;
}

Memory::ImageLoadStatus Memory::load_verilog_8(std::string image_path) {
    std::fstream fin = std::fstream(image_path);
    if (!fin) {
        return ImageLoadStatus::ERROR;
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
                return ImageLoadStatus::ERROR;
            }
            
            //The data word this token represents
            Word data_word = (uint32_t)std::stoul(token, nullptr, 16);

            //Write the data word to memory and increment the address to the next word
            AccessStatus access_status;
            write_memory(addr, DT_BYTE, data_word, access_status);
            if(access_status != AccessStatus::OKAY) {
                return ImageLoadStatus::ERROR;
            }
            ++addr;
        }
    }
    return ImageLoadStatus::OKAY;
}

Memory::ImageLoadStatus Memory::load_verilog_32(std::string image_path) {
    std::fstream fin = std::fstream(image_path);
    if (!fin) {
        return ImageLoadStatus::ERROR;
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
            AccessStatus access_status;
            write_memory(addr, DT_WORD, data_word, access_status);
            if (access_status != AccessStatus::OKAY) {
                return ImageLoadStatus::ERROR;
            }
            addr += 4;
        }
    }
    return ImageLoadStatus::OKAY;
}
