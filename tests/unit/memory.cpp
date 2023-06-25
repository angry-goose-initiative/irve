/* memory.cpp
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Performs unit tests for IRVE's memory.h and memory.cpp
 *
*/

/* Includes */

#include <cassert>
#include "memory.h"
#include "CSR.h"
#include "rvexception.h"

using namespace irve::internal;

/* Function Implementations */

//TODO also test when it is in virtual memory mode (MMU tests)

int test_memory_memory_t_valid_debugaddr() {//None of these should throw an exception
    CSR::CSR_t CSR;
    memory::memory_t memory(CSR);

    memory.store(DEBUGADDR, 0b000, 'I');
    memory.store(DEBUGADDR, 0b000, 'R');
    memory.store(DEBUGADDR, 0b000, 'V');
    memory.store(DEBUGADDR, 0b000, 'E');
    memory.store(DEBUGADDR, 0b000, '\n');

    return 0;
}

//"Byte writes", but anything reads (byte, halfword, word) are tested
int test_memory_memory_t_valid_ramaddrs_bytes() {//None of these should throw an exception
    CSR::CSR_t CSR;
    memory::memory_t memory(CSR);

    for (uint32_t i = 0; i < RAMSIZE; i += 13) {//Way too slow to do every byte (choose a prime number)
        memory.store(i, 0b000, (uint8_t)(i * 123));
    }

    for (uint32_t i = 0; i < RAMSIZE; i += 13) {//Way too slow to do every byte (choose a prime number)
        assert(memory.load(i, 0b100) == (uint8_t)(i * 123));
        //TODO test little endianness here (halfword accesses, word accesses)
        //TODO test load signed here too
    }

    return 0;
}

//"Halfword writes", but anything reads (byte, halfword, word) are tested
int test_memory_memory_t_valid_ramaddrs_halfwords() {//None of these should throw an exception
    CSR::CSR_t CSR;
    memory::memory_t memory(CSR);

    assert(RAMSIZE % 2 == 0);//RAMSIZE must be a multiple of 2
    for (uint32_t i = 0; i < (RAMSIZE / 2); i += 2 * 13) {//Way too slow to do every byte (choose a prime number)
        memory.store(i, 0b001, (uint16_t)(i * 12345));
    }

    for (uint32_t i = 0; i < (RAMSIZE / 2); i += 2 * 13) {//Way too slow to do every byte (choose a prime number)
        assert(memory.load(i, 0b101) == (uint16_t)(i * 12345));
        //TODO test little endianness here (byte accesses, word accesses)
        //TODO test load signed here too
    }

    return 0;
}

//"Word writes", but anything reads (byte, halfword, word) are tested
int test_memory_memory_t_valid_ramaddrs_words() {//None of these should throw an exception
    CSR::CSR_t CSR;
    memory::memory_t memory(CSR);

    assert(RAMSIZE % 4 == 0);//RAMSIZE must be a multiple of 4
    for (uint32_t i = 0; i < (RAMSIZE / 4); i += 4 * 13) {//Way too slow to do every byte (choose a prime number)
        memory.store(i, 0b010, (uint32_t)(i * 987654321));
    }

    for (uint32_t i = 0; i < (RAMSIZE / 4); i += 4 * 13) {//Way too slow to do every byte (choose a prime number)
        assert(memory.load(i, 0b010) == (uint32_t)(i * 987654321));
        //TODO test little endianness here (byte accesses, halfword accesses)
        //TODO test load signed here too
    }

    return 0;
}

int test_memory_memory_t_invalid_debugaddr() {//These should throw exceptions
    CSR::CSR_t CSR;
    memory::memory_t memory(CSR);

    //Invalid accesses at the debug address (some are also misaligned)
    try {
        memory.load(DEBUGADDR, 0b000);
        assert(false);
    } catch (const rvexception::rvexception_t& e) {
        //This should throw an exception of type rvexception_t
        assert(e.cause() == rvexception::cause_t::LOAD_ACCESS_FAULT_EXCEPTION); 
    }

    try {
        memory.load(DEBUGADDR, 0b100);
        assert(false);
    } catch (const rvexception::rvexception_t& e) {
        //This should throw an exception of type rvexception_t
        assert(e.cause() == rvexception::cause_t::LOAD_ACCESS_FAULT_EXCEPTION); 
    }

    try {
        memory.load(DEBUGADDR, 0b001);
        assert(false);
    } catch (const rvexception::rvexception_t& e) {
        //This should throw an exception of type rvexception_t
        //NOTE the priority. This is a misaligned access too, but the exception should be a load access fault
        //since that has a higher priority according to Page 40 of Volume 2 of the RISC-V spec
        assert(e.cause() == rvexception::cause_t::LOAD_ACCESS_FAULT_EXCEPTION); 
    }

    try {
        memory.store(DEBUGADDR, 0b001, 54321);
        assert(false);
    } catch (const rvexception::rvexception_t& e) {
        //This should throw an exception of type rvexception_t
        //NOTE the priority. This is a misaligned access too, but the exception should be a load access fault
        //since that has a higher priority according to Page 40 of Volume 2 of the RISC-V spec
        assert(e.cause() == rvexception::cause_t::STORE_OR_AMO_ACCESS_FAULT_EXCEPTION); 
    }

    try {
        memory.load(DEBUGADDR, 0b101);
        assert(false);
    } catch (const rvexception::rvexception_t& e) {
        //This should throw an exception of type rvexception_t
        //NOTE the priority. This is a misaligned access too, but the exception should be a load access fault
        //since that has a higher priority according to Page 40 of Volume 2 of the RISC-V spec
        assert(e.cause() == rvexception::cause_t::LOAD_ACCESS_FAULT_EXCEPTION); 
    }

    try {
        memory.load(DEBUGADDR, 0b010);
        assert(false);
    } catch (const rvexception::rvexception_t& e) {
        //This should throw an exception of type rvexception_t
        //NOTE the priority. This is a misaligned access too, but the exception should be a load access fault
        //since that has a higher priority according to Page 40 of Volume 2 of the RISC-V spec
        assert(e.cause() == rvexception::cause_t::LOAD_ACCESS_FAULT_EXCEPTION); 
    }

    try {
        memory.store(DEBUGADDR, 0b010, 0xABCDEF01);
        assert(false);
    } catch (const rvexception::rvexception_t& e) {
        //This should throw an exception of type rvexception_t
        //NOTE the priority. This is a misaligned access too, but the exception should be a load access fault
        //since that has a higher priority according to Page 40 of Volume 2 of the RISC-V spec
        assert(e.cause() == rvexception::cause_t::STORE_OR_AMO_ACCESS_FAULT_EXCEPTION); 
    }

    return 0;
}

int test_memory_memory_t_invalid_ramaddrs_misaligned_halfwords() {//Misaligned accesses in the middle of the RAM
    CSR::CSR_t CSR;
    memory::memory_t memory(CSR);

    for (uint32_t i = 0; i < RAMSIZE; i += 607) {//Way too slow to do every byte (choose a prime number)
        uint32_t address = i | 0b1;//Misaligned intentionally
        try {
            memory.store(address, 0b001, (uint16_t)(i * 12345));
            assert(false);
        } catch (const rvexception::rvexception_t& e) {
            //This should throw an exception of type rvexception_t
            printf("cause: %d\n", (uint32_t)e.cause());
            fflush(stdout);
            assert(e.cause() == rvexception::cause_t::STORE_OR_AMO_ADDRESS_MISALIGNED_EXCEPTION); 
        }
        try {
            memory.load(address, 0b101);
            assert(false);
        } catch (const rvexception::rvexception_t& e) {
            //This should throw an exception of type rvexception_t
            printf("cause: %d\n", (uint32_t)e.cause());
            fflush(stdout);
            assert(e.cause() == rvexception::cause_t::LOAD_ADDRESS_MISALIGNED_EXCEPTION);
        }
        try {
            memory.load(address, 0b001);
            assert(false);
        } catch (const rvexception::rvexception_t& e) {
            //This should throw an exception of type rvexception_t
            printf("cause: %d\n", (uint32_t)e.cause());
            assert(e.cause() == rvexception::cause_t::LOAD_ADDRESS_MISALIGNED_EXCEPTION);
        }
    }

    return 0;

}

int test_memory_memory_t_invalid_ramaddrs_misaligned_words() {//Misaligned accesses in the middle of the RAM
    CSR::CSR_t CSR;
    memory::memory_t memory(CSR);

    for (uint32_t misalignment = 0b01; misalignment <= 0b11; ++misalignment) {
        for (uint32_t i = 0; i < RAMSIZE; i += 607) {//Way too slow to do every byte (choose a prime number)
            uint32_t address = (i & ~0b11) | misalignment;//Misaligned intentionally
            try {
                memory.store(address, 0b010, (uint32_t)(i * 12345));
                assert(false);
            } catch (const rvexception::rvexception_t& e) {
                //This should throw an exception of type rvexception_t
                assert(e.cause() == rvexception::cause_t::STORE_OR_AMO_ADDRESS_MISALIGNED_EXCEPTION); 
            }
            try {
                memory.load(address, 0b010);
                assert(false);
            } catch (const rvexception::rvexception_t& e) {
                //This should throw an exception of type rvexception_t
                assert(e.cause() == rvexception::cause_t::LOAD_ADDRESS_MISALIGNED_EXCEPTION);
            }
        }
    }

    return 0;
}

int test_memory_memory_t_invalid_unmappedaddrs_bytes() {
    CSR::CSR_t CSR;
    memory::memory_t memory(CSR);
    //Invalid accesses in unmapped memory (NOT misaligned)
    //TODO
    return 0;
}

int test_memory_memory_t_invalid_unmappedaddrs_halfwords() {
    CSR::CSR_t CSR;
    memory::memory_t memory(CSR);
    //Invalid accesses in unmapped memory (NOT misaligned)
    //TODO
    return 0;
}

int test_memory_memory_t_invalid_unmappedaddrs_words() {
    CSR::CSR_t CSR;
    memory::memory_t memory(CSR);
    //Invalid accesses in unmapped memory (NOT misaligned)
    //TODO
    return 0;
}

int test_memory_memory_t_invalid_unmappedaddrs_misaligned_halfwords() {
    CSR::CSR_t CSR;
    memory::memory_t memory(CSR);
    //Invalid accesses in unmapped memory (also misaligned)
    //TODO
    return 0;
}

int test_memory_memory_t_invalid_unmappedaddrs_misaligned_words() {
    CSR::CSR_t CSR;
    memory::memory_t memory(CSR);
    //Invalid accesses in unmapped memory (also misaligned)
    //TODO
    return 0;
}

int test_memory_pmemory_t_valid_debugaddr() {//None of these should throw an exception
    memory::pmemory_t pmemory;

    pmemory.write_byte(DEBUGADDR, 'I');
    pmemory.write_byte(DEBUGADDR, 'R');
    pmemory.write_byte(DEBUGADDR, 'V');
    pmemory.write_byte(DEBUGADDR, 'E');
    pmemory.write_byte(DEBUGADDR, '\n');

    return 0;
}

int test_memory_pmemory_t_valid_ramaddrs() {//None of these should throw an exception
    memory::pmemory_t pmemory;

    for (uint32_t i = 0; i < RAMSIZE; i += 13) {//Way too slow to do every byte (choose a prime number)
        pmemory.write_byte(i, (uint8_t)(i * 123));
    }

    for (uint32_t i = 0; i < RAMSIZE; i += 13) {//Way too slow to do every byte (choose a prime number)
        assert(pmemory.read_byte(i) == (uint8_t)(i * 123));
    }

    return 0;
}

int test_memory_pmemory_t_invalid_debugaddr() {//This should throw an exception
    memory::pmemory_t pmemory;

    try {
        pmemory.read_byte(DEBUGADDR);
        assert(false);
    } catch (const rvexception::rvexception_t& e) {
        //This should throw an exception of type rvexception_t
        assert(e.cause() == rvexception::cause_t::LOAD_ACCESS_FAULT_EXCEPTION); 
    }

    return 0;
}

int test_memory_pmemory_t_invalid_ram_reads() {//These should throw exceptions
    memory::pmemory_t pmemory;

    for (uint32_t i = RAMSIZE; i < DEBUGADDR; i += 7919) {//Way too slow to do every byte (choose a prime number)
        try {
            pmemory.read_byte(i);
            assert(false);
        } catch (const rvexception::rvexception_t& e) {
            //This should throw an exception of type rvexception_t
            assert(e.cause() == rvexception::cause_t::LOAD_ACCESS_FAULT_EXCEPTION); 
        }

        if ((i + 7919) < i) {//Overflow, thus end of test
            break;
        }
    }

    return 0;
}

int test_memory_pmemory_t_invalid_ram_writes() {//These should throw exceptions
    memory::pmemory_t pmemory;

    for (uint32_t i = RAMSIZE; i < DEBUGADDR; i += 7919) {//Way too slow to do every byte (choose a prime number)
        try {
            pmemory.check_writable_byte(i);
            assert(false);
            pmemory.write_byte(i, 0xA5);
        } catch (const rvexception::rvexception_t& e) {
            //This should throw an exception of type rvexception_t
            assert(e.cause() == rvexception::cause_t::STORE_OR_AMO_ACCESS_FAULT_EXCEPTION); 
        }

        if ((i + 7919) < i) {//Overflow, thus end of test
            break;
        }
    }

    return 0;
}
