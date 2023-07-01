/* cxx_experiments.cpp
 * Copyright (C) 2023 John Jekel
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Experimenting with CXX / CXX bringup in RVSW (on the software side)
 *
*/

/* Constants And Defines */

//TODO

/* Includes */

//#include <iostream>
//#include <cstdio>
#include <functional>
#include <vector>

/* Types */

//TODO

/* Variables */


/* Static Function Declarations */

//TODO

/* Function Implementations */

int main(int, const char**) {
    //*(volatile int*)0xFFFFFFFF = '\n';

    std::vector<std::function<void()>> test_vector;
    test_vector.push_back([]() {
        //std::cout << "CXX Experiments :)" << std::endl;
        puts("CXX Experiments :)");
    });

    //*(volatile int*)0xFFFFFFFF = '\n';
    //puts("Made it here");

    while (!test_vector.empty()) {
        test_vector.back()();
        test_vector.pop_back();
    }

    //TODO add more

    return 0;
}

/* Static Function Implementations */

//TODO
