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

#include <cassert>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <functional>
#include <unordered_map>
#include <vector>

/* Types */

//TODO

/* Static Function Declarations */

void mean();

/* Function Implementations */

int main(int, const char**) {
    //Not just a vector, but a vector that is itself on the heap
    std::vector<std::function<void()>>* test_vector = new std::vector<std::function<void()>>;
    test_vector->push_back([]() {
        std::cout << "Here is a number in hex: 0x" << std::hex << 0x12345678 << std::endl;
    });
    test_vector->push_back([]() {
        std::cout << "But we did it! :)" << std::endl;
    });
    test_vector->push_back([]() {
        std::cout << "So many subtle things to miss..." << std::endl;
    });
    test_vector->push_back([]() {
        std::cout << "Do you know how hard it was to get this working?" << std::endl;
    });
    test_vector->push_back([]() {
        std::cout << "CXX Experiments :)" << std::endl;
    });

    while (!test_vector->empty()) {
        test_vector->back()();
        test_vector->pop_back();
    }

    delete test_vector;
    test_vector = nullptr;

    std::cout << "Let's play with a hash table of my favourite numbers!" << std::endl;

    std::unordered_map<std::string, double> test_map;
    test_map["Pi"] = M_PI;
    test_map["e"] = M_E;
    test_map["not a number"] = NAN;
    test_map["sqrt(2)"] = sqrt(2);
    test_map["g"] = 9.81;
    test_map["c"] = 299792458;
    test_map.erase("not a number");//Wait, that's not a number!

    for (auto& i : test_map) {
        std::cout << i.first << " = " << std::setprecision(16) << i.second << std::endl;
    }

    std::cout << "Notice how the numbers aren't in a particular order! Cool!" << std::endl;

    try {
        mean();
        assert(false && "If this prints, that means the throw didn't work");
    } catch (std::exception& e) {
        std::cout << "Caught exception: " << e.what() << std::endl;
    }

    return 0;
}

extern "C" __attribute__ ((interrupt ("machine"))) void ___rvsw_exception_handler___(void) {
    assert(false && "We don't expect any exceptions to occur in this test program");
    exit(1);
}

/* Static Function Implementations */

void mean() {
    std::cout << "Inside mean()" << std::endl;
    throw std::runtime_error("I am a meany function >:(");//FIXME why do we die here?
    assert(false && "If this prints, that means the throw didn't work");
}
