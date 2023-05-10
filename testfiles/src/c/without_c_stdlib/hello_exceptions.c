/* hello_exceptions.c
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Early experiments with RISC-V exception handling
 *
 * Based on code from rv32esim
*/

/* Includes */

#include "irve.h"

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
    print_string("Playing with RISC-V exceptions!\n");

    __asm__ volatile("ecall");

    assert(false && "TESTING");
}

void __interrupt_and_trap_handler() {
    print_string("Hey would you look at that, an exception!\n");
    irve_exit();
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
    irve_exit();
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
