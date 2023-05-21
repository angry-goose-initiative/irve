/* debugging_puts_weirdness.c
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Exploring weirdness with puts()
 *
*/

/* Includes */

#include "irve.h"

#include <stdio.h>

/* Function Implementations */

int main() {
    puts("This is a call to puts");
    puts("So is this. But neither was flushed automatically which is quite odd.");
    puts("After this (another puts call), let's flush manually.");
    fflush(stdout);
    puts("Weird right? Why are those lines ending with a null byte rather than a newline?");
    puts("You won't see that text either right away. Let's add a newline manually then to this line to cause a flush.\n");

    puts("Curiously after the newline there is still a null byte! And again this line (printed by puts) won't be flushed right away!");
    puts("But if you uncomment the printf call below, all of the null bytes everywhere BECOME newlines! What the heck?");
    //printf("WTH!? We already flushed a couple times, how in the world are those times affected by this future call?");

    //TODO maybe this is a Verilog hex file loader issue?

    puts("This will be _interesting_ to figure out...");
    return 0;
}
