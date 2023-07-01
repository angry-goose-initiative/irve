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
#include <list>
#include <regex>
#include <unordered_map>
#include <vector>
#include <queue>

/* Types */

//TODO

/* Static Function Declarations */

static void example_regex_code_from_cppreference();
static void mean();

/* Function Implementations */

int main(int, const char**) {
    //Not just a vector, but a vector that is itself on the heap
    std::vector<std::function<void()>>* test_vector = new std::vector<std::function<void()>>;
    test_vector->push_back([]() {
        std::cout << "Here is a number in hex: 0x" << std::hex << 0x12345678 << std::dec << std::endl;
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

    std::cout << "------------------------------------------------------------------------------------------" << std::endl;

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

    std::cout << "------------------------------------------------------------------------------------------" << std::endl;

    std::cout << "Copying all number out of the hash table into a list..." << std::endl;
    std::list<double> test_list;
    for (auto& i : test_map) {
        test_list.push_back(i.second);
    }
    std::cout << "Sorting the list..." << std::endl;
    test_list.sort();
    std::cout << "Printing the list..." << std::endl;
    for (auto& i : test_list) {
        std::cout << std::setprecision(16) << i << std::endl;
    }
    std::cout << "The list is " << std::distance(test_list.begin(), test_list.end()) << " numbers long" << std::endl;

    std::cout << "------------------------------------------------------------------------------------------" << std::endl;

    std::cout << "Copying all number out of the hash table into a priority queue..." << std::endl;
    std::priority_queue<double> test_queue;
    for (auto& i : test_map) {
        test_queue.push(i.second);
    }
    std::cout << "Printing the queue..." << std::endl;
    while (!test_queue.empty()) {
        std::cout << std::setprecision(16) << test_queue.top() << std::endl;
        test_queue.pop();
    }

    std::cout << "------------------------------------------------------------------------------------------" << std::endl;


    std::cout << "Trying out the example regex code from cppreference.com..." << std::endl;
    example_regex_code_from_cppreference();
    std::cout << "Finished trying out the example regex code" << std::endl;

    std::cout << "------------------------------------------------------------------------------------------" << std::endl;

    std::cout << "Playing with exceptions now..." << std::endl;
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

static void example_regex_code_from_cppreference() {
    //Thanks https://en.cppreference.com/w/cpp/regex
    std::string s = "Some people, when confronted with a problem, think "
        "\"I know, I'll use regular expressions.\" "
        "Now they have two problems.";
 
    std::regex self_regex("REGULAR EXPRESSIONS",
            std::regex_constants::ECMAScript | std::regex_constants::icase);
    if (std::regex_search(s, self_regex)) {
        std::cout << "Text contains the phrase 'regular expressions'\n";
    }
 
    std::regex word_regex("(\\w+)");
    auto words_begin = 
        std::sregex_iterator(s.begin(), s.end(), word_regex);
    auto words_end = std::sregex_iterator();
 
    std::cout << "Found "
              << std::distance(words_begin, words_end)
              << " words\n";
    
    assert((std::distance(words_begin, words_end) == 20) && "We expect 20 words in the string");
 
    const int N = 6;
    std::cout << "Words longer than " << N << " characters:\n";
    for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
        std::smatch match = *i;
        std::string match_str = match.str();
        if (match_str.size() > N) {
            std::cout << "  " << match_str << '\n';
        }
    }
 
    std::regex long_word_regex("(\\w{7,})");
    std::string new_s = std::regex_replace(s, long_word_regex, "[$&]");
    std::cout << new_s << '\n';
}

static void mean() {
    std::cout << "Inside mean()" << std::endl;
    throw std::runtime_error("I am a meany function >:(");//FIXME why do we die here?
    assert(false && "If this prints, that means the throw didn't work");
}
