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
    namespace emulator { class emulator_t; }

    namespace loader {
        void load_verilog_32(emulator::emulator_t& emulator, const char* filename);//TODO return false if this fails
    }

    namespace logging {
        void log(uint8_t indent, const char* str, ...);
        void log_always(uint8_t indent, const char* str, ...);//USE THIS SPARINGLY
        bool logging_disabled();
    }

    namespace about {
        std::size_t get_version_major();
        std::size_t get_version_minor();
        std::size_t get_version_patch();
        const char* get_version_string();
        const char* get_build_time_string();
        const char* get_build_date_string();
        const char* get_build_system_string();
        const char* get_build_host_string();
        const char* get_compile_target_string();
        const char* get_compiler_string();
    }

    //Things that depend on previous declarations

    //We have to do it this way to maintain ABI compatibility: https://en.cppreference.com/w/cpp/language/pimpl
    namespace internal::emulator { class emulator_t; }//Forward declaration of the internal class

    namespace emulator {
        //We have to do it this way to maintain ABI compatibility: https://en.cppreference.com/w/cpp/language/pimpl
        class emulator_t {//TODO provide read-only access to the CPU state at the end for integration testing
        public:
            emulator_t();
            ~emulator_t();

            bool tick();//Returns true as long as the emulator should continue running

            //Runs the emulator until the given instruction count is reached or an exit request is made
            //For dynamic linking to libirve, this is more efficient than calling tick() in a loop
            void run_until(uint64_t inst_count);

            uint64_t get_inst_count() const;

            uint8_t mem_read_byte(uint32_t addr) const;
            void mem_write_byte(uint32_t addr, uint8_t data);

            //TODO how to best expose CPU state to the user for them to modify/read? (particularly for integration testing)
        private:
            friend void irve::loader::load_verilog_32(emulator_t& emulator, const char* filename);
            irve::internal::emulator::emulator_t* m_emulator_ptr;
        };
    }
}

#endif//COMMON_H
