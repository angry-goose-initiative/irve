/* memory.cpp
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 * 
 * A class for the memory (RAM, not MMIO) of the emulator
 *
 * TODO change this class to deal with virtual memory when the CPU is in user mode and
 * to actually in fact deal with MMIO too
 * 
*/

/* Includes */

#include "memory.h"

#include <cassert>
#include <iostream>
#include <cstring>
#include <memory>

#include "CSR.h"

#include "common.h"
#include "rvexception.h"

#define INST_COUNT 0//We only log at init
#include "logging.h"

using namespace irve::internal;

/* Function Implementations */

// `memory_t` Function Implementations

memory::memory_t::memory_t(CSR::CSR_t& CSR_ref):
        m_mem(),
        m_CSR_ref(CSR_ref) {
    irvelog(1, "Created new Memory instance");
}

word_t memory::memory_t::instruction(word_t addr) {
    uint64_t machine_addr = translate_address(addr, AT_INSTRUCTION);

    return read_physical(machine_addr, DT_WORD);
}

word_t memory::memory_t::load(word_t addr, uint8_t data_type) {
    assert((data_type <= 0b111) && "Invalid funct3");

    uint64_t machine_addr = translate_address(addr, AT_LOAD);

    return read_physical(machine_addr, data_type);
}

void memory::memory_t::store(word_t addr, uint8_t data_type, word_t data) {
    assert((data_type <= 0b010) && "Invalid funct3");

    uint64_t machine_addr = translate_address(addr, AT_STORE);

    // 2^(funct3[1:0]) is the number of bytes
    int8_t byte = (int8_t)spow(2, data_type & 0b11);

    // Check that all bytes are writable before any byte is written to
    for(int i = 0; i<byte; ++i) {
        this->m_mem.check_writable_byte(machine_addr + i);
    }

    // If all bytes are writable, then write to each byte
    for(int i = 0; i<byte; ++i) {
        this->m_mem.write_byte(machine_addr + i, (uint8_t)data.srl(8 * i).u);
    }

    // Check for misaligned access
    // Note that this happens AFTER writing to physical memory because access faults take
    // priority over misaligned faults
    if (((data_type & 0b11) == 0b001) && ((addr.u % 2) != 0)) {
        // Misaligned halfword read
        invoke_rv_exception(LOAD_ADDRESS_MISALIGNED);
    }
    else if ((data_type == 0b010) && ((addr.u % 4) != 0)) {
        // Misaligned word read
        invoke_rv_exception(LOAD_ADDRESS_MISALIGNED);
    }
}

uint64_t memory::memory_t::translate_address(word_t untranslated_addr, uint8_t access_type) const {
    if(1) { //TODO condition for no address translation
        return (uint64_t)untranslated_addr.u;
    }
    int i = 1;
    uint64_t machine_addr{};
    uint32_t a = satp_PPN * PAGESIZE;
    uint64_t pte_addr = a + va_VPN(untranslated_addr, i) * 4;
    word_t pte = read_physical(pte_addr, DT_WORD);
    // If the pte is not valid or the page is writable and not readable, raise a page-fault
    // exception corresponding to the access type
    if(pte_V(pte) == 0 || (pte_R(pte) == 0 && pte_W(pte) == 1)) {
        invoke_rv_exception_by_num((rvexception::cause_t)(PAGE_FAULT_BASE + access_type));
    }
    // TODO more steps
    machine_addr = untranslated_addr.bits(12,0).u;

    return machine_addr;
}

word_t memory::memory_t::read_physical(uint64_t addr, uint8_t data_type) const {

    // 2^(funct3[1:0]) is the number of bytes
    int8_t byte = (int8_t)(spow(2, data_type & 0b11) - 1);

    word_t data;

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
    if (((data_type & 0b11) == 0b001) && ((addr % 2) != 0)) {
        // Misaligned halfword read
        invoke_rv_exception(LOAD_ADDRESS_MISALIGNED);
    }
    else if ((data_type == 0b010) && ((addr % 4) != 0)) {
        // Misaligned word read
        invoke_rv_exception(LOAD_ADDRESS_MISALIGNED);
    }

    return data;
}

// `pmemory_t` Function Implementations

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
            if (addr >= RAMSIZE) {
                assert(false && "Use `check_writable_byte` first");
            }
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
