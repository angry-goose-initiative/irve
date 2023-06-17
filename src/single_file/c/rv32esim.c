/* rv32esim.c
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Simple test code 
 *
 * Based on code from rv32esim
*/

/* Includes */

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>

/* Function Implementations */

int main() {
    //printf("Testing");
    puts("Hello world! C running on RISC-V!\n");
    //print_string("\xAA\xBB\xCC\xDD\xEE\xFF");
    volatile uint32_t a = 1;
    volatile uint32_t b = 2;

    assert((a + b) == 3);
    assert((a - b) == 0xFFFFFFFF);
    assert((a << b) == 4);
    assert((a >> b) == 0);
    assert(a < b);
    volatile int32_t ai = 0x80000000;
    volatile uint32_t bi = 3;
    printf("0x%X", ai >> bi);
    putc('\n');
    assert((uint32_t)(ai >> bi) == 0xF0000000);

    assert((b * bi) == 6);
    //assert(false && "Sanity check assert works");

    return 0;
}
