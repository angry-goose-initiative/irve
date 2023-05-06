/* rv32im_sanity.c
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * IRVE test code for the base RISC-V spec and M extension
 *
 * Based on code from rv32esim
*/

/* Includes */

#include "irve.h"

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>

/* Static Function Declarations */

static void print_string(const char* str);
static void print_uint(uint64_t uint);
static void print_uint_bin(uint32_t uint);
void test_iterations(uint32_t cycles);

/* Function Implementations */

int main() {
    print_string("Sanity test for base RISC-V spec and M extension\n");

    print_string("Testing base spec\n");
    volatile uint32_t a = 1;
    volatile uint32_t b = 2;
    assert((a + b) == 3);
    assert((a - b) == 0xFFFFFFFF);
    assert((a << b) == 4);
    assert((a >> b) == 0);
    assert(a < b);
    volatile int32_t ai = 0x80000000;
    volatile uint32_t bi = 3;
    print_uint_bin(ai >> bi);//Arithmetic shift right
    print_string("\n");
    assert((uint32_t)(ai >> bi) == 0xF0000000);
    
    //Test how well lui works (hopefully)
    volatile uint32_t c = 0x1234ABCD;
    assert(c == 0x1234ABCD);

    //TODO test more base spec stuff

    print_string("Testing M extension\n");

    volatile uint32_t zero = 0;
    volatile int32_t zeroi = 0;
    assert((a * b) == 2);
    assert((a / b) == 0);
    assert((a % b) == 1);
    assert((a / zero) == 0xFFFFFFFF);//This is what should occur according to the spec
    assert((a % zero) == a);//This is what should occur according to the spec
    assert((ai / zeroi) == -1);//This is what should occur according to the spec
    assert((ai % zeroi) == ai);//This is what should occur according to the spec
    volatile int32_t will_cause_overflow = -2147483648;
    assert((will_cause_overflow / -1) == will_cause_overflow);//This is what should occur according to the spec
    assert((will_cause_overflow % -1) == 0);//This is what should occur according to the spec
    
    //TODO test more M extension stuff
}

//Called if an assertion fails
void __assert_func(const char* file, int line, const char* function, const char* expr) {
    print_string("Assertion failed: ");
    print_string(file);
    print_string(" | ");
    print_string(function);
    print_string(" | ");
    print_string(expr);
    print_string("\n");
    __asm__ volatile ("ecall");
    while (true);
}

/* Static Function Implementations */

static void print_string(const char* str) {
    volatile char test = *str;
    while (*str) {
        IRVE_DEBUG_ADDR = *str;
        ++str;
    }
}

static void print_uint(uint64_t uint) {//TODO do this more efficiently
    char buffer[32];
    buffer[31] = 0x00;//Null terminator

    uint64_t index = 30;
    while (uint) {
        buffer[index] = '0' + (uint % 10);

        --index;
        uint /= 10;
    }
    print_string(&buffer[index + 1]);
}

static void print_uint_bin(uint32_t uint) {
    IRVE_DEBUG_ADDR = '0';
    IRVE_DEBUG_ADDR = 'b';

    uint32_t mask = 1ull << 31;
    bool first_one_encountered = false;
    while (mask) {

        if (uint & mask) {
            IRVE_DEBUG_ADDR = '1';
            first_one_encountered = true;
        } else if (first_one_encountered) {
            IRVE_DEBUG_ADDR = '0';
        }

        mask >>= 1;
    }

    if (!first_one_encountered) {
        IRVE_DEBUG_ADDR = '0';
    }
}

void test_iterations(uint32_t iterations) {
    for (uint32_t i = 0; i < iterations; ++i)
        __asm__ volatile("nop");
}
