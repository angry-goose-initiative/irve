/* irve_public_api.h
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Public API for IRVE (all that is exposed to the unit tester and the irve executable)
 *
 * ONLY external programs and irve_public_api.cpp should include this file
 *
*/

#ifndef IRVE_PUBLIC_API_H
#define IRVE_PUBLIC_API_H

/* Includes */

//CANNOT contain any private headers

#include <cstddef>
#include <cstdint>

/* Types */

/* Function/Class Declarations */

//Note: Everything must refer to a symbol in the irve library, not be a #define constant!
//This is to support dynamic linking with different libirve.so versions
//Or, if using static linking, to avoid the need to rebuild integration tests and the irve executable when ex. the logging is disabled or the version changes

namespace irve {//NOT irve::internal
    namespace emulator {
        //TODO
        //TODO do this for emulator_t: https://en.cppreference.com/w/cpp/language/pimpl
    }

    namespace loader {
        //void load_verilog_32(emulator::emulator_t& emulator, const char* filename);
    }

    namespace logging {
        void log(uint8_t indent, const char* str, ...);
        bool logging_disabled();
    }

    namespace about {
        std::size_t get_version_major();
        std::size_t get_version_minor();
        std::size_t get_version_patch();
        const char* get_version_string();
        const char* get_build_time_string();
        const char* get_build_date_string();
    }
}

#endif//COMMON_H
