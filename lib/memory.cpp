/* memory.cpp
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * The classes for the memory of the emulator
 *
 * Manages virtual memory and physical memory
 * 
*/

/* Includes */

#include "memory.h"

#include <cassert>
#include <iostream>
#include <cstring>
#include <memory>
#include <fstream>
#include <string>

#include "CSR.h"

#include "common.h"
#include "rvexception.h"

#define INST_COUNT 0//We only log at init
#include "logging.h"

using namespace irve::internal;

/* `pmemory_t` Function Implementations */

memory::pmemory_t::pmemory_t():
        m_ram(new uint8_t[RAMSIZE]) {
    irvelog(1, "Created new physical memory instance");
    std::memset(this->m_ram.get(), 0, RAMSIZE);
}

memory::pmemory_t::~pmemory_t() {
    if (this->m_debugstr.size() > 0) {
        irvelog_always_stdout(0, "\x1b[92mRISC-V Remaining Debug At IRVE Exit:\x1b[0m: \"\x1b[1m%s\x1b[0m\"", this->m_debugstr.c_str());
    }
}

uint8_t memory::pmemory_t::read_byte(uint64_t addr) const {
    //TODO add mtime and mtimecmp registers

    if (addr >= RAMSIZE) {
        invoke_rv_exception(LOAD_ACCESS_FAULT);
    }

    //TODO add MMIO devices that provide data as things progress
    
    return this->m_ram[addr];
}

void memory::pmemory_t::write_byte(uint64_t addr, uint8_t data) {
    //TODO other MMIO devices

    switch (addr) {
        case DEBUGADDR:
            //End of line; print the debug string
            if (data == '\n') {
                irvelog_always_stdout(0, "\x1b[92mRISC-V Says\x1b[0m: \"\x1b[1m%s\x1b[0m\\n\"", this->m_debugstr.c_str());
                this->m_debugstr.clear();
            }
            else if (data == '\0') {
                irvelog_always_stdout(0, "\x1b[92mRISC-V Says\x1b[0m: \"\x1b[1m%s\x1b[0m\\0\"", this->m_debugstr.c_str());
                this->m_debugstr.clear();
            }
            else {
                this->m_debugstr.push_back((char)data);
            }
            break;
        default:
            // Not MMIO
            this->m_ram[addr] = data;
            break;
    }
}

void memory::pmemory_t::check_writable_byte(uint64_t addr) {
    switch (addr) {
        case DEBUGADDR:
            break;
        default:
            // Not MMIO
            if (addr >= RAMSIZE) {
                invoke_rv_exception(STORE_OR_AMO_ACCESS_FAULT);
            }
            break;
    }
}

/* `memory_t` Function Implementations */

memory::memory_t::memory_t(CSR::CSR_t& CSR_ref):
        m_mem(),
        m_CSR_ref(CSR_ref) {
    irvelog(1, "Created new Memory instance");
}

memory::memory_t::memory_t(int imagec, char** imagev, CSR::CSR_t& CSR_ref):
        m_mem(),
        m_CSR_ref(CSR_ref) {
    try {
        load_memory_image_files(imagec, imagev);
    }
    catch(...) {
        // TODO make this more descriptive?
        throw std::exception();
    }
    irvelog(1, "Created new Memory instance");
}

word_t memory::memory_t::instruction(word_t addr) {
    uint64_t machine_addr = translate_address(addr, AT_INSTRUCTION);

    word_t data = read_physical(machine_addr, DT_WORD);

    // Throw an exception if the PC is not aligned to a word boundary
    if ((machine_addr % 4) != 0) {
        invoke_rv_exception(INSTRUCTION_ADDRESS_MISALIGNED);
    }

    return data;
}

word_t memory::memory_t::load(word_t addr, uint8_t data_type) {
    assert((data_type <= 0b111) && "Invalid funct3");
    assert((data_type != 0b110) && "Invalid funct3");

    uint64_t machine_addr = translate_address(addr, AT_LOAD);

    return read_physical(machine_addr, data_type);
}

void memory::memory_t::store(word_t addr, uint8_t data_type, word_t data) {
    assert((data_type <= 0b010) && "Invalid funct3");

    uint64_t machine_addr = translate_address(addr, AT_STORE);

    write_physical(machine_addr, data_type, data);
}

uint64_t memory::memory_t::translate_address(word_t untranslated_addr, uint8_t access_type) const {
    if(satp_MODE == 0) { // TODO check the condition
        // No address translation
        return (uint64_t)untranslated_addr.u;
    }

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

    int i = 1;
    while(1) {
        // STEP 2
        pte_addr = a + (va_VPN(i) * 4);
        // This access may raise an access-fault exception
        // TODO ensure this exeption corresponds to the original access type
        pte = read_physical(pte_addr, DT_WORD);

        assert(pte_G == 0 && "Global bit was set by software (but we haven't implemented it)");

        // STEP 3
        if(pte_V == 0 || (pte_R == 0 && pte_W == 1)) {
            // The pte is not valid or the page is writable and not readable
            invoke_rv_exception_by_num((rvexception::cause_t)(PAGE_FAULT_BASE + access_type));
        }

        // STEP 4
        if(pte_R == 1 || pte_X == 1) {
            // Leaf pte found
            break;
        }
        else {
            --i;
            if(i < 0) {
                invoke_rv_exception_by_num((rvexception::cause_t)(PAGE_FAULT_BASE + access_type));
            }
        }
    }

    // STEP 5
    if(ACCESS_NOT_ALLOWED) {
        invoke_rv_exception_by_num((rvexception::cause_t)(PAGE_FAULT_BASE + access_type));
    }

    // STEP 6
    if((i == 1) && (pte_PPN0 != 0)) {
        // Misaligned superpage
        invoke_rv_exception_by_num((rvexception::cause_t)(PAGE_FAULT_BASE + access_type));
    }

    // STEP 7
    if((pte_A == 0) || ((access_type == AT_STORE) && (pte_D == 0))) {
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

    return machine_addr;
}

word_t memory::memory_t::read_physical(uint64_t addr, uint8_t data_type) const {
    // 2^(funct3[1:0]) is the number of bytes
    int8_t byte = (int8_t)(spow(2, data_type & DATA_WIDTH_MASK) - 1);

    word_t data = 0;

    for(; byte > -1; --byte) {
        data |= this->m_mem.read_byte(addr + byte) << (byte * 8);
    }

    // Perform sign extension if necessary
    if (data_type == DT_SIGNED_BYTE) {
        data = data.sign_extend_from_size(8);
    }
    else if (data_type == DT_SIGNED_HALFWORD) {
        data = data.sign_extend_from_size(16);
    }

    // Check for misaligned access
    // Note that this happens AFTER reading from physical memory because access faults take
    // priority over misaligned faults
    if (((data_type & DATA_WIDTH_MASK) == DT_HALFWORD) && ((addr & 0b1) != 0)) {
        // Misaligned halfword read
        invoke_rv_exception(LOAD_ADDRESS_MISALIGNED);
    }
    else if ((data_type == DT_WORD) && ((addr & 0b11) != 0)) {
        // Misaligned word read
        invoke_rv_exception(LOAD_ADDRESS_MISALIGNED);
    }

    return data;
}

void memory::memory_t::write_physical(uint64_t addr, uint8_t data_type, word_t data) {
    // 2^(funct3[1:0]) is the number of bytes
    int8_t byte = (int8_t)spow(2, data_type & DATA_WIDTH_MASK);

    // Check that all bytes are writable before any byte is written to
    for(int i = 0; i<byte; ++i) {
        this->m_mem.check_writable_byte(addr + i);
    }

    // If all bytes are writable, then write to each byte
    for(int i = 0; i<byte; ++i) {
        this->m_mem.write_byte(addr + i, (uint8_t)data.srl(8 * i).u);
    }

    // Check for misaligned access
    // Note that this happens AFTER writing to physical memory because access faults take priority
    // over misaligned faults
    if (((data_type & DATA_WIDTH_MASK) == DT_HALFWORD) && ((addr & 0b1) != 0)) {
        // Misaligned halfword read
        invoke_rv_exception(LOAD_ADDRESS_MISALIGNED);
    }
    else if ((data_type == DT_WORD) && ((addr & 0b11) != 0)) {
        // Misaligned word read
        invoke_rv_exception(LOAD_ADDRESS_MISALIGNED);
    }
}

void memory::memory_t::load_memory_image_files(int imagec, char** imagev) {
    //FIXME don't handle the first argument of argv here, assume it has already been cut off

    // Ensure that a memory file image was specified
    /* if(imagec == 1) {
        irvelog_always(0, "Error: No memory image file specified!");
        throw std::exception();
    }*/
    // Load each memory file
    for(int i = 1; i < imagec; ++i) {
        std::string path = imagev[i];
        if (path.find("/") == std::string::npos) {
            path = TESTFILES_DIR + path;
        }
        irvelog_always(0, "Loading memory image from file \"%s\"", path.c_str());
        load_verilog_32(path);
    }
}

void memory::memory_t::load_verilog_32(std::string image_path) {
    std::fstream fin = std::fstream(image_path);
    assert(fin && "Failed to open memory image file");

    // Read the file token by token
    uint64_t addr = 0;
    std::string token;
    while (fin >> token) {
        assert((token.length() != 0) && "This should never happen");
        if (token.at(0) == '@') { // `@` indicates a new address (ASSUMING 32-BIT WORDS)
            std::string new_addr_str = token.substr(1);
            assert((new_addr_str.length() == 8) && "Memory image file is not formatted correctly (bad address)");
            addr = std::stoul(new_addr_str, nullptr, 16);
            addr *= 4; // This is a word address, not a byte address, so multiply by 4
        }
        else { // New data word (32-bit, could be an instruction or data)
            if (token.length() != 8) {
                irvelog(1, "Warning: 32-bit Verilog image file is not formatted correctly (data word is not 8 characters long). This is likely an objcopy bug. Continuing anyway...");
            }
            
            // The data word this token represents
            word_t data_word = (uint32_t)std::stoul(token, nullptr, 16);

            // Write the data word to memory and increment the address to the next word
            write_physical(addr, DT_WORD, data_word);
            addr += 4;
        }
    }
}
