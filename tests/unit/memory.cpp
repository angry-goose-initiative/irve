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

int test_memory_t() {
    memory_t memory;

    //TODO

    return 0;
}

int test_pmemory_t() {
    pmemory_t pmemory;

    //None of these should throw an exception
    pmemory.w(DEBUGADDR, 'I');
    pmemory.w(DEBUGADDR, 'R');
    pmemory.w(DEBUGADDR, 'V');
    pmemory.w(DEBUGADDR, 'E');
    pmemory.w(DEBUGADDR, '\n');

    for (uint32_t i = 0; i < RAMSIZE; ++i) {
        pmemory.w(i, (uint8_t)(i * 123));
    }

    for (uint32_t i = 0; i < RAMSIZE; ++i) {
        assert(pmemory.r(i) == (uint8_t)(i * 123));
    }

    //These should throw exceptions

    try {
        pmemory.r(DEBUGADDR);
        assert(false);
    } catch (const rvexception_t& e) {
        //This should throw an exception of type rvexception_t
        assert(!e.is_interrupt());
        assert(e.cause() == LOAD_ACCESS_FAULT_EXCEPTION); 
    }

    try {
        pmemory.r(RAMSIZE);
        assert(false);
    } catch (const rvexception_t& e) {
        //This should throw an exception of type rvexception_t
        assert(!e.is_interrupt());
        assert(e.cause() == LOAD_ACCESS_FAULT_EXCEPTION); 
    }

    try {
        pmemory.w(RAMSIZE, 0xA5);
        assert(false);
    } catch (const rvexception_t& e) {
        //This should throw an exception of type rvexception_t
        assert(!e.is_interrupt());
        assert(e.cause() == STORE_OR_AMO_ACCESS_FAULT_EXCEPTION); 
    }

    return 0;
}
