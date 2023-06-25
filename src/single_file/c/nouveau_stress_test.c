/* nouveau_stress_test.c
 * Copyright (C) 2023 John Jekel
 * See the LICENSE file at the root of the project for licensing info.
 *
 * A much larger RVSW stress test
 *
*/

/* Constants And Defines */

#ifdef RVSW_MMODE
#define MODE "M-Mode"
#else
#define MODE "S-Mode"
#endif

#define MAX_ITERATIONS 100

/* Includes */

#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <complex.h>
#include <stdint.h>

/* Types */

//TODO

/* Static Function Declarations */

static uint64_t efficient_fibonacci(uint64_t n);
static uint64_t inefficient_recursive_fibonacci(uint64_t n);

static void print_mandelbrot();
static uint64_t mandelbrot_iterations(double complex point);

/* Function Implementations */

int main() {
    printf("Nouveau Stress Test (%s)\n", MODE);

    for (uint64_t i = 0; i < 20; i++) {
        printf("Fibonacci %llu: %llu %llu\n", i, efficient_fibonacci(i), inefficient_recursive_fibonacci(i));
    }

    print_mandelbrot();
    
    return 0;
}

/* Static Function Implementations */

static uint64_t efficient_fibonacci(uint64_t n) {
    uint64_t prevprev = 0;
    uint64_t prev = 1;

    for (uint64_t i = 0; i < n; i++) {
        uint64_t current = prevprev + prev;
        prevprev = prev;
        prev = current;
    }

    return prevprev;
}

static uint64_t inefficient_recursive_fibonacci(uint64_t n) {
    if (n == 0) {
        return 0;
    } else if (n == 1) {
        return 1;
    } else {
        return inefficient_recursive_fibonacci(n - 2) + inefficient_recursive_fibonacci(n - 1);
    }
}

static void print_mandelbrot() {
    double starting_real = -2;
    double starting_imaginary = -1;
    double ending_real = 1;
    double ending_imaginary = 1;
    double real_step = (ending_real - starting_real) / 60;
    double imaginary_step = (ending_imaginary - starting_imaginary) / 20;

    double imag = starting_imaginary;
    for (uint64_t i = 0; i < 20; ++i) {
        double real = starting_real;
        for (uint64_t j = 0; j < 60; ++j) {
            uint64_t result = mandelbrot_iterations((double complex)(real + (imag * I)));

            if (result >= (MAX_ITERATIONS / 2)) {
                putc(' ', stdout);
            } else {
                putc('*', stdout);
            }

            real += real_step;
        }
        printf("\n");
        
        imag += imaginary_step;
    }
}

static uint64_t mandelbrot_iterations(double complex point) {
    double complex z = 0;
    for (uint64_t i = 0; i < MAX_ITERATIONS; ++i) {
        if (cabs(z) > 2) {
            return i;
        }

        z = (z * z) + point;

        if (i == MAX_ITERATIONS - 1) {
            return MAX_ITERATIONS;
        }
    }

    assert(false && "We should never get here");
}

__attribute__ ((interrupt ("machine"))) void ___rvsw_exception_handler___(void) {
    assert(false && "We don't expect any exceptions to occur in this test program");
    exit(1);
}
