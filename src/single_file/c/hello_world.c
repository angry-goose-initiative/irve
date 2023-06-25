/* hello_world.c
 * Copyright (C) 2023 John Jekel
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Prints "Hello World!" and exits.
 *
*/

/* Includes */

#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

/* Function Implementations */

int main(void)
{
    puts("Hello World!");
    return 0;
}

__attribute__ ((interrupt ("machine"))) void ___rvsw_exception_handler___(void) {
    assert(false && "We don't expect any exceptions to occur in this test program");
    exit(1);
}
