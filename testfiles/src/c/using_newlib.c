/* using_newlib.c
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Trying to use newlib
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

//TODO

/* Function Implementations */

int main() {
    //TODO currently this dosn't compile since we haven't setup any Newlib syscalls
    puts("Hello World with Newlib!");
}

//Called if an assertion fails
/*void __assert_func(const char* file, int line, const char* function, const char* expr) {
    print_string("Assertion failed: ");
    print_string(file);
    print_string(" | ");
    print_string(function);
    print_string(" | ");
    print_string(expr);
    print_string("\n");
    __asm__ volatile ("ecall");
    while (true);
}*/

/* Static Function Implementations */

//TODO
