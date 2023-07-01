/* cxx_experiments.c
 * Copyright (C) 2023 John Jekel
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Experimenting with CXX / CXX bringup in RVSW (on the software side)
 *
*/

/* Constants And Defines */

//TODO

/* Includes */

#include <cstdio>
#include <functional>
#include <vector>

/* Types */

//TODO

/* Variables */

void* __dso_handle = nullptr;//FIXME we shouldn't have to do this

/* Static Function Declarations */

//TODO

/* Function Implementations */

int main(int, const char**) {
    std::vector<std::function<void()>> test_vector;
    test_vector.push_back([]() {
        puts("CXX Experiments :)");
    });
    test_vector.back()();

    //TODO add more

    return 0;
}

/* Static Function Implementations */

//TODO
