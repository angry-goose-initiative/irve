/* software_floating_point_fun.c
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Stress-testing with software floating point provided by libgcc
 *
 * Based on code from rv32esim
*/

/* Constants And Defines */

#define TOLERABLE_ERROR 0.0000000000001

/* Includes */

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

/* Function Implementations */

int main() {
    puts("Testing software floating point!\n");

    volatile double a = 12.34;
    volatile double b = 5.678;
    volatile double zero = 0.0;
    volatile double pi = M_PI;
    volatile double nan = NAN;

    printf("a is %f\n", a);
    printf("b is %f\n", b);
    printf("0 is %f\n", zero);
    printf("pi is %f\n", pi);
    printf("nan contains %f\n", nan);
    printf("a + b is %f\n", a + b);
    printf("a - b is %f\n", a - b);
    printf("a * b is %f\n", a * b);
    printf("a / b is %f\n", a / b);
    printf("sin(zero) is %1.40f\n", sin(zero));
    printf("cos(zero) is %1.40f\n", cos(zero));
    printf("sin(pi) is %1.40f\n", sin(pi));
    printf("cos(pi) is %1.40f\n", cos(pi));

    assert(a == a);
    assert(b == b);
    assert(zero == zero);
    assert(pi == pi);
    assert(nan != nan);

    assert(a == 12.34);
    assert(b == 5.678);
    assert(zero == 0.0);
    assert(pi == M_PI);
    assert(nan != NAN);
    assert((a + b) == 18.018);
    assert((a - b) == 6.662);
    assert((a * b) == 70.06652);
    assert((a / b) == 2.1733004579077142004);
    assert(sin(0) == 0.0);
    assert(cos(0) == 1.0);
    assert(sin(pi) == 0.0000000000000001224646799147353207173764);//Not exactly 0 when I run it on my laptop
    assert(cos(pi) == -1.0);

    //TODO test other math functions

    puts("Testing sqrt and cbrt");
    for (volatile double i = 123.456; i < 567.890; i += 9.876) {
        assert(sqrt(i) == pow(i, 0.5));
        assert(abs(cbrt(i) - pow(i, 1.0 / 3.0)) < TOLERABLE_ERROR);
    }

    puts("Testing log and exp");
    for (volatile double i = 123.456; i < 567.890; i += 9.876) {
        volatile double temp = exp(i);
        assert(log(temp) == i);
        temp = log(i);
        assert(abs(exp(temp) - i) < TOLERABLE_ERROR);
        temp = exp2(i);
        assert(abs(log2(temp) - i) < TOLERABLE_ERROR);
        temp = log2(i);
        assert(abs(exp2(temp) - i) < TOLERABLE_ERROR);
    }

    //TODO test others (ex cos and acos)

    return 0;
}

__attribute__ ((interrupt ("machine"))) void ___rvsw_exception_handler___(void) {
    assert(false && "We don't expect any exceptions to occur in this test program");
    exit(1);
}
