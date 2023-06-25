/* rv32im_sanity.c
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Test code for the base RISC-V spec and M extension
 *
 * Based on code from rv32esim
*/

/* Includes */

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

/* Function Implementations */

int main() {
    puts("Sanity test for base RISC-V spec and M extension\n");

    puts("Testing base spec\n");
    volatile uint32_t a = 1;
    volatile uint32_t b = 2;
    assert((a + b) == 3);
    assert((a - b) == 0xFFFFFFFF);
    assert((a << b) == 4);
    assert((a >> b) == 0);
    assert(a < b);
    volatile int32_t ai = 0x80000000;
    volatile uint32_t bi = 3;
    printf("0x%lX\n", ai >> bi);//Arithmetic shift right
    assert((uint32_t)(ai >> bi) == 0xF0000000);
    
    //Test how well lui works (hopefully)
    volatile uint32_t c = 0x1234ABCD;
    assert(c == 0x1234ABCD);

    //TODO test more base spec stuff

    puts("Testing M extension\n");

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

    return 0;
}

__attribute__ ((interrupt ("machine"))) void ___rvsw_exception_handler___(void) {
    assert(false && "We don't expect any exceptions to occur in this test program");
    exit(1);
}
