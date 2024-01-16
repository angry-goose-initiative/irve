/**
 * @file    hello_cxx.cpp
 * @brief   Prints "Hello World!" and exits.
 * 
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  See the LICENSE file at the root of the project for licensing info.
*/

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include <cassert>
#include <cstdlib>
#include <cstdio>//iostream is hard, we'll make a seperate file for that

/* ------------------------------------------------------------------------------------------------
 * Type/Class Declarations
 * --------------------------------------------------------------------------------------------- */

class hello_world_t {
    public:
        hello_world_t(void);
};

/* ------------------------------------------------------------------------------------------------
 * Static Variables
 * --------------------------------------------------------------------------------------------- */

static bool static_contructor_called = false;

static hello_world_t hello_world;

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

int main(void) {
    puts("Hello World from main()!");
    assert(static_contructor_called && "The static constructor was not called!");
    return 0;
}

hello_world_t::hello_world_t(void) {
    puts("Hello World from hello_world_t::hello_world_t()!");
    static_contructor_called = true;
}

extern "C" __attribute__ ((interrupt ("machine"))) void ___rvsw_exception_handler___(void) {
    assert(false && "We don't expect any exceptions to occur in this test program");
    exit(1);
}
