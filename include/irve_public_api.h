/* irve_public_api.h
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Public API for IRVE (all that is exposed to the unit tester and the irve executable)
 *
*/

#ifndef IRVE_PUBLIC_API_H
#define IRVE_PUBLIC_API_H

/* Includes */

#include <cstddef>

/* Types */

/* Function/Class Declarations */

namespace irve {
    //TODO do this for execute_t: https://en.cppreference.com/w/cpp/language/pimpl

    namespace about {
        std::size_t get_version_major();
        std::size_t get_version_minor();
        std::size_t get_version_patch();
        const char* get_version_string();
        bool logging_disabled();
    }
}

#endif//COMMON_H
