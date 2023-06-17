/* software_floating_point_fun.c
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Stress-testing IRVE with software floating point provided by libgcc
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
static void print_double(double d);
static void print_uint(uint64_t uint);
void test_iterations(uint32_t cycles);

/* Function Implementations */

int main() {
    print_string("Testing software floating point!\n");

    volatile double a = 12.34;
    volatile double b = 5.678;

    assert(a == 12.34);
    assert(b == 5.678);
    assert((a + b) == 18.018);
    assert((a - b) == 6.662);
    assert((a * b) == 70.06652);
    assert((a / b) == 2.1733004579077142004);

    print_string("a is ");
    print_double(a);
    print_string("\nb is ");
    print_double(b);
    print_string("\na + b is ");
    print_double(a + b);
    print_string("\na - b is ");
    print_double(a - b);
    print_string("\na * b is ");
    print_double(a * b);
    print_string("\na / b is ");
    print_double(a / b);
    print_string("\n");

    return 0;
}

//Called if an assertion fails
void __assert_func(const char* file, int, const char* function, const char* expr) {
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
    while (*str) {
        IRVE_DEBUG_ADDR = *str;
        ++str;
    }
}

static void print_double(double d) {
    //NOTE this is a very inefficient way to do this and it dosn't even print the exactly correct value
    int64_t integer_part = (int64_t)d;
    print_uint((uint64_t)integer_part);
    print_string(".");
    d -= integer_part;
    while (d) {
        d *= 10;
        integer_part = (int64_t)d;
        print_uint((uint64_t)integer_part);
        d -= integer_part;
    }
}

static void print_uint(uint64_t uint) {//TODO do this more efficiently
    if (!uint) {
        IRVE_DEBUG_ADDR = '0';
        return;
    } 

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

void test_iterations(uint32_t iterations) {
    for (uint32_t i = 0; i < iterations; ++i)
        __asm__ volatile("nop");
}
