/**
 * @file    memory.cpp
 * @brief   Unit tests for IRVE's memory.h & memory.cpp
 * 
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  Copyright (C) 2023 Nick Chan\n
 *  See the LICENSE file at the root of the project for licensing info.
 * 
 * TODO longer description
 *
*/

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

// We do this so we can access internal emulator state for testing
#define private public

#include <cassert>
#include "memory.h"
#include "csr.h"
#include "rvexception.h"
#include "memory_map.h"
#include "common.h"

using namespace irve::internal;

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

// Test that user ram is little endian
int test_memory_Memory_user_ram_endianness() {
    Csr CSR;
    Memory memory(CSR);
    
    // Check endianness for words
    memory.store((uint32_t)MEM_MAP_REGION_START_USER_RAM, DT_WORD, 0x00010203);
    assert(memory.load((uint32_t)MEM_MAP_REGION_START_USER_RAM + 0, DT_UNSIGNED_BYTE) == (uint32_t)0x03);
    assert(memory.load((uint32_t)MEM_MAP_REGION_START_USER_RAM + 1, DT_UNSIGNED_BYTE) == (uint32_t)0x02);
    assert(memory.load((uint32_t)MEM_MAP_REGION_START_USER_RAM + 2, DT_UNSIGNED_BYTE) == (uint32_t)0x01);
    assert(memory.load((uint32_t)MEM_MAP_REGION_START_USER_RAM + 3, DT_UNSIGNED_BYTE) == (uint32_t)0x00);

    // Check endianness for halfwords
    memory.store((uint32_t)MEM_MAP_REGION_START_USER_RAM, DT_HALFWORD, 0x0000F0F1);
    assert(memory.load((uint32_t)MEM_MAP_REGION_START_USER_RAM + 0, DT_UNSIGNED_BYTE) == (uint32_t)0xF1);
    assert(memory.load((uint32_t)MEM_MAP_REGION_START_USER_RAM + 1, DT_UNSIGNED_BYTE) == (uint32_t)0xF0);

    return 0;
}

// Test sign extending for user ram
int test_memory_Memory_user_ram_sign_extending() {
    Csr CSR;
    Memory memory(CSR);

    memory.store((uint32_t)MEM_MAP_REGION_START_USER_RAM, DT_HALFWORD, 0x00008080);

    // Unsigned byte load should zero extend
    assert(memory.load((uint32_t)MEM_MAP_REGION_START_USER_RAM, DT_UNSIGNED_BYTE) == 0x00000080);
    // Signed byte load should sign extend
    assert(memory.load((uint32_t)MEM_MAP_REGION_START_USER_RAM, DT_SIGNED_BYTE) == 0xFFFFFF80);
    // Unsigned halfword load should zero extend
    assert(memory.load((uint32_t)MEM_MAP_REGION_START_USER_RAM, DT_UNSIGNED_HALFWORD) == 0x00008080);
    // Signed halfword load should sign extend
    assert(memory.load((uint32_t)MEM_MAP_REGION_START_USER_RAM, DT_SIGNED_HALFWORD) == 0xFFFF8080);

    return 0;
}

// Test that valid accesses to user ram do not raise exceptions
int test_memory_Memory_user_ram_valid_byte_access() {
    Csr CSR;
    Memory memory(CSR);

    // It's way too slow to check every byte so we choose a prime number
    for (uint64_t i = MEM_MAP_REGION_START_USER_RAM; i <= MEM_MAP_REGION_END_USER_RAM; i += 13) {
        memory.store((uint32_t)i, DT_BYTE, (uint8_t)(i * 123));
    }
    for (uint64_t i = MEM_MAP_REGION_START_USER_RAM; i <= MEM_MAP_REGION_END_USER_RAM; i += 13) {
        assert(memory.load((uint32_t)i, DT_UNSIGNED_BYTE) == (uint8_t)(i * 123));
    }

    return 0;
}

// Test that valid stores to the debug address don't raise exceptions
int test_memory_Memory_valid_debugaddr() {
    Csr CSR;
    Memory memory(CSR);

    memory.store((uint32_t)MEM_MAP_ADDR_DEBUG, DT_BYTE, 'I');
    memory.store((uint32_t)MEM_MAP_ADDR_DEBUG, DT_BYTE, 'R');
    memory.store((uint32_t)MEM_MAP_ADDR_DEBUG, DT_BYTE, 'V');
    memory.store((uint32_t)MEM_MAP_ADDR_DEBUG, DT_BYTE, 'E');
    memory.store((uint32_t)MEM_MAP_ADDR_DEBUG, DT_BYTE, '\n');

    return 0;
}

// "Halfword writes", but anything reads (byte, halfword, word) are tested
int test_memory_Memory_valid_ramaddrs_halfwords() {//None of these should throw an exception
    Csr CSR;
    Memory memory(CSR);

    // (uint32_t)MEM_MAP_REGION_SIZE_USER_RAM must be a multiple of 2
    assert((uint32_t)MEM_MAP_REGION_SIZE_USER_RAM % 2 == 0);

    // It's way too slow to check every byte so we choose a prime number
    for (uint32_t i = 0; i < ((uint32_t)MEM_MAP_REGION_SIZE_USER_RAM / 2); i += 2 * 13) {
        memory.store(i, DT_HALFWORD, (uint16_t)(i * 12345));
    }
    for (uint32_t i = 0; i < ((uint32_t)MEM_MAP_REGION_SIZE_USER_RAM / 2); i += 2 * 13) {
        assert(memory.load(i, DT_UNSIGNED_HALFWORD) == (uint16_t)(i * 12345));
    }

    return 0;
}

// "Word writes", but anything reads (byte, halfword, word) are tested
int test_memory_Memory_valid_ramaddrs_words() {//None of these should throw an exception
    Csr CSR;
    Memory memory(CSR);

    // (uint32_t)MEM_MAP_REGION_SIZE_USER_RAM must be a multiple of 4
    assert((uint32_t)MEM_MAP_REGION_SIZE_USER_RAM % 4 == 0);
    
    // It's way too slow to check every byte so we choose a prime number
    for (uint32_t i = 0; i < ((uint32_t)MEM_MAP_REGION_SIZE_USER_RAM / 4); i += 4 * 13) {
        memory.store(i, DT_WORD, (uint32_t)(i * 987654321));
    }
    for (uint32_t i = 0; i < ((uint32_t)MEM_MAP_REGION_SIZE_USER_RAM / 4); i += 4 * 13) {
        assert(memory.load(i, DT_WORD) == (uint32_t)(i * 987654321));
    }

    return 0;
}

// Test that invalid accesses to the debug address throw exceptions
int test_memory_Memory_invalid_debugaddr() {
    Csr CSR;
    Memory memory(CSR);

    // The debug address is write-only. Attempting to read from it violates PMA check and causes a
    // load access-fault exception
    try {
        memory.load((uint32_t)MEM_MAP_ADDR_DEBUG, DT_UNSIGNED_BYTE);
        assert(false);
    } catch (const rvexception::rvexception_t& e) {
        assert(e.cause() == rvexception::cause_t::LOAD_ACCESS_FAULT_EXCEPTION); 
    }

    // The debug address can only be accessed as a byte. Halfword and word access violates PMA
    // check and causes an access-fault exception. Note that this is also a misaligned address, but
    // access-fault exceptions take priority over address-misaligned exceptions according to page
    // 40 of the RISC-V Spec Volume 2
    try {
        memory.store((uint32_t)MEM_MAP_ADDR_DEBUG, DT_HALFWORD, 0xABCD);
        assert(false);
    } catch (const rvexception::rvexception_t& e) {
        assert(e.cause() == rvexception::cause_t::STORE_OR_AMO_ACCESS_FAULT_EXCEPTION); 
    }

    try {
        memory.store((uint32_t)MEM_MAP_ADDR_DEBUG, DT_WORD, 0xABCDEF01);
        assert(false);
    } catch (const rvexception::rvexception_t& e) {
        assert(e.cause() == rvexception::cause_t::STORE_OR_AMO_ACCESS_FAULT_EXCEPTION); 
    }

    return 0;
}

// Test that misaligned halfword accesses to user ram cause exceptions
int test_memory_Memory_invalid_ramaddrs_misaligned_halfwords() {
    Csr CSR;
    Memory memory(CSR);

    // It's way too slow to check every byte so we choose a prime number
    for (uint32_t i = 0; i < (uint32_t)MEM_MAP_REGION_SIZE_USER_RAM; i += 607) {
        // Intentionally misalign the address
        uint32_t address = i | 0b1;

        try {
            memory.store(address, DT_HALFWORD, (uint16_t)(i * 12345));
            assert(false);
        } catch (const rvexception::rvexception_t& e) {
            assert(e.cause() == rvexception::cause_t::STORE_OR_AMO_ADDRESS_MISALIGNED_EXCEPTION);
        }
        try {
            memory.load(address, DT_UNSIGNED_HALFWORD);
            assert(false);
        } catch (const rvexception::rvexception_t& e) {
            assert(e.cause() == rvexception::cause_t::LOAD_ADDRESS_MISALIGNED_EXCEPTION);
        }
        try {
            memory.load(address, DT_SIGNED_HALFWORD);
            assert(false);
        } catch (const rvexception::rvexception_t& e) {
            assert(e.cause() == rvexception::cause_t::LOAD_ADDRESS_MISALIGNED_EXCEPTION);
        }
    }

    return 0;

}

// Test that misaligned word accesses to user ram cause exceptions
int test_memory_Memory_invalid_ramaddrs_misaligned_words() {
    Csr CSR;
    Memory memory(CSR);

    for (uint32_t misalignment = 0b01; misalignment <= 0b11; ++misalignment) {
        // It's way too slow to check every byte so we choose a prime number
        for (uint32_t i = 0; i < (uint32_t)MEM_MAP_REGION_SIZE_USER_RAM; i += 607) {
            uint32_t address = (i & ~0b11) | misalignment;//Misaligned intentionally
            try {
                memory.store(address, DT_WORD, (uint32_t)(i * 12345));
                assert(false);
            } catch (const rvexception::rvexception_t& e) {
                assert(e.cause() == rvexception::cause_t::STORE_OR_AMO_ADDRESS_MISALIGNED_EXCEPTION); 
            }
            try {
                memory.load(address, DT_WORD);
                assert(false);
            } catch (const rvexception::rvexception_t& e) {
                assert(e.cause() == rvexception::cause_t::LOAD_ADDRESS_MISALIGNED_EXCEPTION);
            }
        }
    }

    return 0;
}

int test_memory_Memory_invalid_unmappedaddrs_bytes() {
    Csr CSR;
    Memory memory(CSR);
    //Invalid accesses in unmapped memory (NOT misaligned)
    //TODO
    return 0;
}

int test_memory_Memory_invalid_unmappedaddrs_halfwords() {
    Csr CSR;
    Memory memory(CSR);
    //Invalid accesses in unmapped memory (NOT misaligned)
    //TODO
    return 0;
}

int test_memory_Memory_invalid_unmappedaddrs_words() {
    Csr CSR;
    Memory memory(CSR);
    //Invalid accesses in unmapped memory (NOT misaligned)
    //TODO
    return 0;
}

int test_memory_Memory_invalid_unmappedaddrs_misaligned_halfwords() {
    Csr CSR;
    Memory memory(CSR);
    //Invalid accesses in unmapped memory (also misaligned)
    //TODO
    return 0;
}

int test_memory_Memory_invalid_unmappedaddrs_misaligned_words() {
    Csr CSR;
    Memory memory(CSR);
    //Invalid accesses in unmapped memory (also misaligned)
    //TODO
    return 0;
}

// Tests that the address translation scheme is being chosen correctly
int test_memory_Memory_translation_conditions() {
    Csr CSR;
    Memory memory(CSR);
    
    // Start in M-mode
    CSR.set_privilege_mode(PrivilegeMode::MACHINE_MODE);
    // Start with MPRV set to 0
    CSR.implicit_write(Csr::Address::MSTATUS, Word(0b00000000000000000001100000000000));
    // Start with satp indicating bare
    CSR.implicit_write(Csr::Address::SATP, Word(0x00000000));

    // No translation should occur
    assert(memory.no_address_translation(1));

    // MPP set to S-mode
    CSR.implicit_write(Csr::Address::MSTATUS, Word(0b00000000000000000000100000000000));

    // No translation should occur
    assert(memory.no_address_translation(1));

    // MPRV set to 1
    CSR.implicit_write(Csr::Address::MSTATUS, Word(0b00000000000000100000100000000000));

    // No translation should occur
    assert(memory.no_address_translation(1));

    // satp indicates sv32
    CSR.implicit_write(Csr::Address::SATP, Word(0x80000000));

    // Translation should occur for a load and a store...
    assert(!memory.no_address_translation(1) && !memory.no_address_translation(2));
    // ...but not for an instruction fetch
    assert(memory.no_address_translation(0));

    // satp indicates bare
    CSR.implicit_write(Csr::Address::SATP, Word(0x00000000));

    // No translation should occur
    assert(memory.no_address_translation(2));

    // MPP set to 0
    CSR.implicit_write(Csr::Address::MSTATUS, Word(0b00000000000000000000100000000000));

    // Switch to S-mode
    CSR.set_privilege_mode(PrivilegeMode::SUPERVISOR_MODE);

    // No translation should occur
    assert(memory.no_address_translation(0));

    // satp incicates sv32
    CSR.implicit_write(Csr::Address::SATP, (Word)0x80000000);

    // Translation should occur
    assert(!memory.no_address_translation(1));

    // MPP set to M-mode, MPRV set to 1
    CSR.implicit_write(Csr::Address::MSTATUS, Word(0b00000000000000100001100000000000));

    // Translation shouldn't occur for a load and a store...
    assert(memory.no_address_translation(1) && memory.no_address_translation(2));
    // ...but should for an instruction fetch
    assert(!memory.no_address_translation(0));

    // MPRV set to 0
    CSR.implicit_write(Csr::Address::MSTATUS, Word(0b00000000000000000001100000000000));

    // Switch to U-mode
    CSR.set_privilege_mode(PrivilegeMode::USER_MODE);

    // Translation should occur
    assert(!memory.no_address_translation(1));

    // satp indicates bare
    CSR.implicit_write(Csr::Address::SATP, Word(0x00000000));

    // No translation should occur
    assert(memory.no_address_translation(0));

    return 0;
}

int test_memory_Memory_supervisor_loads_with_translation() {
    Csr CSR;
    Memory memory(CSR);

    // First level pte at 0x00000000
    Word FIRST_LEVEL_PTE_ADDR = 0x00000000;
    // pte valid bit set
    // pte.PPN = 0x1
    Word pte1 = 0x00000401;

    // Second level pte at 0x00001F00
    Word SECOND_LEVEL_PTE_ADDR = 0x00001F00;
    // pte valid, readable, writable, accessed bit set
    // pte.PPN = 0x4
    Word pte2 = 0x00001047;

    // Only working with S-mode here
    CSR.set_privilege_mode(PrivilegeMode::SUPERVISOR_MODE);
    // Starts with bare address translation
    CSR.implicit_write(Csr::Address::SATP, Word(0x00000000));

    // Write first level pte to memory
    memory.store(FIRST_LEVEL_PTE_ADDR, DT_WORD, pte1);
    // Write second level pte to memory
    memory.store(SECOND_LEVEL_PTE_ADDR, DT_WORD, pte2);

    // Write the data that will be read after translation to memory
    memory.store(0x00004FF0, DT_WORD, 0x1234ABCD);

    // Switch to SV32 address translation
    CSR.implicit_write(Csr::Address::SATP, Word(0x80000000));

    // va.VPN[1] = 0x0
    // va.VPN[0] = 0b1111000000  (0x3C0)
    // va.offset = 0xFF0
    Word va = 0x003C0FF0;

    assert(memory.load(va, DT_WORD).u == 0x1234ABCD);


    // Loading a superpage

    // First level pte at 0x0010A1F24 or 0b 00 0000 0001 0000 1010 0001 1111 0010 0100
    FIRST_LEVEL_PTE_ADDR = 0x0010A1F24;
    // pte valid, readable, accessed bit set (superpage)
    // pte.PPN = 0x0
    pte1 = 0x00000043;

    // Return bare address translation
    CSR.implicit_write(Csr::Address::SATP, Word(0x00000000));

    // Write first level pte to memory
    memory.store(FIRST_LEVEL_PTE_ADDR, DT_WORD, pte1);

    // Write the data that will be read after translation to memory
    memory.store(0x000E0960, DT_WORD, 0xABCD1234);

    // Switch to SV32 address translation
    // satp.PPN = 0x10A1
    CSR.implicit_write(Csr::Address::SATP, Word(0x800010A1));

    // va.VPN[1] = 0b0010100001
    // va.VPN[0] = 0xE0
    // va.offset = 0x960
    va = 0xF24E0960;

    assert(memory.load(va, DT_WORD).u == 0xABCD1234);

    return 0;
}
