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
#include "rvexception.h"

/* Function Implementations */

int test_memory_t_valid() {//None of these should throw an exception
    memory_t memory;

    //TODO also test when it is in virtual memory mode

    memory.w(DEBUGADDR, 0b000, 'I');
    memory.w(DEBUGADDR, 0b000, 'R');
    memory.w(DEBUGADDR, 0b000, 'V');
    memory.w(DEBUGADDR, 0b000, 'E');
    memory.w(DEBUGADDR, 0b000, '\n');

    for (uint32_t i = 0; i < RAMSIZE; i += 13) {//Way too slow to do every byte (choose a prime number)
        memory.w(i, 0b000, (uint8_t)(i * 123));
    }

    for (uint32_t i = 0; i < RAMSIZE; i += 13) {//Way too slow to do every byte (choose a prime number)
        assert(memory.r(i, 0b000) == (uint8_t)(i * 123));
        //TODO test little endianness here (halfword accesses, word accesses)
        //TODO test load signed here too
    }

    assert(RAMSIZE % 2 == 0);//RAMSIZE must be a multiple of 2
    for (uint32_t i = 0; i < (RAMSIZE / 2); i += 2 * 13) {//Way too slow to do every byte (choose a prime number)
        memory.w(i, 0b001, (uint16_t)(i * 12345));
    }

    for (uint32_t i = 0; i < (RAMSIZE / 2); i += 2 * 13) {//Way too slow to do every byte (choose a prime number)
        assert(memory.r(i, 0b101) == (uint16_t)(i * 12345));
        //TODO test little endianness here (byte accesses, word accesses)
        //TODO test load signed here too
    }

    assert(RAMSIZE % 4 == 0);//RAMSIZE must be a multiple of 4
    for (uint32_t i = 0; i < (RAMSIZE / 4); i += 4 * 13) {//Way too slow to do every byte (choose a prime number)
        memory.w(i, 0b010, (uint32_t)(i * 987654321));
    }

    for (uint32_t i = 0; i < (RAMSIZE / 4); i += 4 * 13) {//Way too slow to do every byte (choose a prime number)
        assert(memory.r(i, 0b010) == (uint32_t)(i * 987654321));
        //TODO test little endianness here (byte accesses, halfword accesses)
        //TODO test load signed here too
    }

    return 0;
}

int test_memory_t_invalid() {//These should throw exceptions
    memory_t memory;

    //Invalid accesses at the debug address (some are also misaligned)
    try {
        memory.r(DEBUGADDR, 0b000);
        assert(false);
    } catch (const rvexception_t& e) {
        //This should throw an exception of type rvexception_t
        assert(!e.is_interrupt());
        assert(e.cause() == LOAD_ACCESS_FAULT_EXCEPTION); 
    }

    try {
        memory.r(DEBUGADDR, 0b100);
        assert(false);
    } catch (const rvexception_t& e) {
        //This should throw an exception of type rvexception_t
        assert(!e.is_interrupt());
        assert(e.cause() == LOAD_ACCESS_FAULT_EXCEPTION); 
    }

    try {
        memory.r(DEBUGADDR, 0b001);
        assert(false);
    } catch (const rvexception_t& e) {
        //This should throw an exception of type rvexception_t
        //NOTE the priority. This is a misaligned access too, but the exception should be a load access fault
        //since that has a higher priority according to Page 40 of Volume 2 of the RISC-V spec
        assert(!e.is_interrupt());
        assert(e.cause() == LOAD_ACCESS_FAULT_EXCEPTION); 
    }

    try {
        memory.w(DEBUGADDR, 0b001, 54321);
        assert(false);
    } catch (const rvexception_t& e) {
        //This should throw an exception of type rvexception_t
        //NOTE the priority. This is a misaligned access too, but the exception should be a load access fault
        //since that has a higher priority according to Page 40 of Volume 2 of the RISC-V spec
        assert(!e.is_interrupt());
        assert(e.cause() == STORE_OR_AMO_ACCESS_FAULT_EXCEPTION); 
    }

    try {
        memory.r(DEBUGADDR, 0b101);
        assert(false);
    } catch (const rvexception_t& e) {
        //This should throw an exception of type rvexception_t
        //NOTE the priority. This is a misaligned access too, but the exception should be a load access fault
        //since that has a higher priority according to Page 40 of Volume 2 of the RISC-V spec
        assert(!e.is_interrupt());
        assert(e.cause() == LOAD_ACCESS_FAULT_EXCEPTION); 
    }

    try {
        memory.r(DEBUGADDR, 0b010);
        assert(false);
    } catch (const rvexception_t& e) {
        //This should throw an exception of type rvexception_t
        //NOTE the priority. This is a misaligned access too, but the exception should be a load access fault
        //since that has a higher priority according to Page 40 of Volume 2 of the RISC-V spec
        assert(!e.is_interrupt());
        assert(e.cause() == LOAD_ACCESS_FAULT_EXCEPTION); 
    }

    try {
        memory.w(DEBUGADDR, 0b010, 0xABCDEF01);
        assert(false);
    } catch (const rvexception_t& e) {
        //This should throw an exception of type rvexception_t
        //NOTE the priority. This is a misaligned access too, but the exception should be a load access fault
        //since that has a higher priority according to Page 40 of Volume 2 of the RISC-V spec
        assert(!e.is_interrupt());
        assert(e.cause() == STORE_OR_AMO_ACCESS_FAULT_EXCEPTION); 
    }

    //Misaligned accesses in the middle of the RAM
    //TODO
    
    //Invalid accesses in unmapped memory (some are also misaligned)
    //TODO
    
    return 0;
}

int test_pmemory_t_valid() {//None of these should throw an exception
    pmemory_t pmemory;

    pmemory.w(DEBUGADDR, 'I');
    pmemory.w(DEBUGADDR, 'R');
    pmemory.w(DEBUGADDR, 'V');
    pmemory.w(DEBUGADDR, 'E');
    pmemory.w(DEBUGADDR, '\n');

    for (uint32_t i = 0; i < RAMSIZE; i += 13) {//Way too slow to do every byte (choose a prime number)
        pmemory.w(i, (uint8_t)(i * 123));
    }

    for (uint32_t i = 0; i < RAMSIZE; i += 13) {//Way too slow to do every byte (choose a prime number)
        assert(pmemory.r(i) == (uint8_t)(i * 123));
    }

    return 0;
}

int test_pmemory_t_invalid() {//These should throw exceptions
    pmemory_t pmemory;

    try {
        pmemory.r(DEBUGADDR);
        assert(false);
    } catch (const rvexception_t& e) {
        //This should throw an exception of type rvexception_t
        assert(!e.is_interrupt());
        assert(e.cause() == LOAD_ACCESS_FAULT_EXCEPTION); 
    }

    for (uint32_t i = RAMSIZE; i < DEBUGADDR; i += 7919) {//Way too slow to do every byte (choose a prime number)
        try {
            pmemory.r(i);
            assert(false);
        } catch (const rvexception_t& e) {
            //This should throw an exception of type rvexception_t
            assert(!e.is_interrupt());
            assert(e.cause() == LOAD_ACCESS_FAULT_EXCEPTION); 
        }
    }

    for (uint32_t i = RAMSIZE; i < DEBUGADDR; i += 7919) {//Way too slow to do every byte (choose a prime number)
        try {
            pmemory.w(i, 0xA5);
            assert(false);
        } catch (const rvexception_t& e) {
            //This should throw an exception of type rvexception_t
            assert(!e.is_interrupt());
            assert(e.cause() == STORE_OR_AMO_ACCESS_FAULT_EXCEPTION); 
        }
    }

    return 0;
}
