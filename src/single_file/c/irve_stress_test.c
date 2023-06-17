/* stress_test.c
 * Copyright (C) 2023 John Jekel
 * See the LICENSE file at the root of the project for licensing info.
 *
 * A stress test originally from IRVE
 *
*/

/* Constants And Defines */

#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Includes */

//TODO

/* Types */

//TODO

/* Variables */

//extern int __executable_start;
//extern int __stack_top;
//extern int __SDATA_BEGIN__;
//extern int __heap_start__;

/* Static Function Declarations */

//static void fp_math();

/* Function Implementations */

int main() {
    printf("Hello %s!\n", "World");
    printf("Testing %d\n", 123);
    printf("This is used to be broken and print a lot of zeroes. Now it prints 123.456! : %f\n", 123.456);
    
    printf("Let's look at some cool stuff with addresses!\n");
    printf("Address of main: %p\n", (char*) &main);
    //printf("Address of __executable_start: %p\n", (char*) &__executable_start);
    //printf("Address of __stack_top: %p\n", (char*) &__stack_top);
    //printf("Address of __SDATA_BEGIN__: %p\n", (char*) &__SDATA_BEGIN__);
    //printf("Address of __heap_start__: %p\n", (char*) &__heap_start__);

    printf("Let's see if Newlib's dynamic memory routines work!\n");
    for (int i = 0; i < 10; i++) {
        int size = (i * 123) + 2;
        char* test_of_dynamic_memory_that_should_never_be_done_on_an_embedded_system_but_is_being_done_for_testing_purposes = (char*)malloc((i * 123) + 2);
        assert(test_of_dynamic_memory_that_should_never_be_done_on_an_embedded_system_but_is_being_done_for_testing_purposes != NULL);
        printf("Address of dynamic memory: %p\n", test_of_dynamic_memory_that_should_never_be_done_on_an_embedded_system_but_is_being_done_for_testing_purposes);
        strncpy(test_of_dynamic_memory_that_should_never_be_done_on_an_embedded_system_but_is_being_done_for_testing_purposes, "This is a test of dynamic memory", size);//FIXME or should this be size - 1 or + 1 instead?
        test_of_dynamic_memory_that_should_never_be_done_on_an_embedded_system_but_is_being_done_for_testing_purposes[size - 1] = '\0';
        printf("Dynamic memory: %s\n", test_of_dynamic_memory_that_should_never_be_done_on_an_embedded_system_but_is_being_done_for_testing_purposes);
        free(test_of_dynamic_memory_that_should_never_be_done_on_an_embedded_system_but_is_being_done_for_testing_purposes);

        //TODO add another allocation to test how the heap allocations move around
    }

    volatile double pi = M_PI;
    printf("The square root of pi is %f\n", sqrt(pi));

    //for (int i = 0; i < 1000; ++i) {
    //    puts("TESTING123\n");
    //}

    //TODO do more
    
    return 0;
}

/* Static Function Implementations */

//TODO
