/* irve_debugging_puts_weirdness.c
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Exploring weirdness with puts()
 *
*/

/* Includes */

#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

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
    //printf("WTH!? We already flushed a couple times, how in the world are those times affected by this future call?\n")

    //TODO maybe this is a Verilog hex file loader issue?//It in fact was!

    puts("This will be _interesting_ to figure out...");


    //Haha! Yep, turns out it was a loader issue!
    //At one point in the Verilog hex file we didn't get a whole word as we should have, but rather a half word
    //This may be some sort of bug is objcopy?
    //
    //Part of the resulting hex file:
    //[...]
    //00000000 73696854 6C697720 6562206C
    //6E695F20 65726574 6E697473 74205F67
    //6966206F 65727567 74756F20 002E2E2E
    //000A <- The problem. Why is objcopy emitting a half word here?
    //@00000D24
    //00000014 00000000 00527A03 01017C01
    //07020D1B 00000001 00000010 0000001C
    //FFFFCB84 00000020 00000000
    //@00000D2F
    //000034D0
    //@00000D30
    //000034D0 FFFFFFFF 00020000
    //@00000D34
    //00000000 000037BC 00003824 0000388C
    //00000000 00000000 00000000 00000000
    //
    //If we change that 000A to 0000000A, the problem goes away!
    //So we will work around this in our loader code
    return 0;
}

__attribute__ ((interrupt ("machine"))) void ___rvsw_exception_handler___(void) {
    assert(false && "We don't expect any exceptions to occur in this test program");
    exit(1);
}
