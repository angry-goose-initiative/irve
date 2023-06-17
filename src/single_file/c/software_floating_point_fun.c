/* software_floating_point_fun.c
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Stress-testing with software floating point provided by libgcc
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
    puts("Testing software floating point!\n");

    volatile double a = 12.34;
    volatile double b = 5.678;

    assert(a == 12.34);
    assert(b == 5.678);
    assert((a + b) == 18.018);
    assert((a - b) == 6.662);
    assert((a * b) == 70.06652);
    assert((a / b) == 2.1733004579077142004);

    printf("a is %f\n", a);
    printf("b is %f\n", b);
    printf("a + b is %f\n", a + b);
    printf("a - b is %f\n", a - b);
    printf("a * b is %f\n", a * b);
    printf("a / b is %f\n", a / b);

    return 0;
}
