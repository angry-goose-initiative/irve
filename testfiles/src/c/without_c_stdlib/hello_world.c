/* hello_world.c
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Prints "Hello World!" using the debug address.
 *
*/

/* Constants And Defines */

const char* const HELLO_WORLD = "Hello World!";

/* Includes */

#include "irve.h"

/* Static Function Declarations */

void my_puts(const char* str);

/* Function Implementations */

int main(void)
{
    my_puts(HELLO_WORLD);
    return 0;
}

/* Static Function Implementations */

void my_puts(const char* str) {
    for (; *str != '\0'; ++str) {
        IRVE_DEBUG_ADDR = *str;
    }
    IRVE_DEBUG_ADDR = '\n';
}
