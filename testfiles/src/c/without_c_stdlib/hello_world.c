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
    const char* char_ptr = str;
    for (const char* char_ptr = HELLO_WORLD; *char_ptr != '\0'; ++char_ptr) {
        IRVE_DEBUG_ADDR = *char_ptr;
    }
    IRVE_DEBUG_ADDR = '\n';
}
