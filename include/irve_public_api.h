/**
 * @file    irve_public_api.h
 * @brief   Public API for IRVE
 * 
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  Copyright (C) 2023 Nick Chan\n
 *  See the LICENSE file at the root of the project for licensing info.
 *
 * Public API for IRVE (all that is exposed to the unit tester and the irve executable)
 *
 * ONLY external programs (frontends) and irve_public_api.cpp should include this file
 *
*/

#pragma once

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

//CANNOT contain any private headers

#include <cstddef>
#include <cstdint>

/* ------------------------------------------------------------------------------------------------
 * Type/Class Declarations
 * --------------------------------------------------------------------------------------------- */

//Note: Everything must refer to a symbol in the irve library, not be a #define constant!
//This is to support dynamic linking with different libirve.so versions
//Or, if using static linking, to avoid the need to rebuild integration tests and the irve executable when ex. the logging is disabled or the version changes

/**
 * @brief The main irve namespace
*/
namespace irve {//NOT irve::internal
    namespace emulator { class emulator_t; }

    /**
     * @brief Contains functions to log messages to the console
    */
    namespace logging {
        /**
         * @brief Log to stderr, when logging is enabled
         * @param indent The indentation level to use
         * @param str The format string to use
         * @param ... The arguments to the format string
        */
        void log(uint8_t indent, const char* str, ...);

        /**
         * @brief Log to stderr, regardless of whether logging is enabled or not
         * @param indent The indentation level to use
         * @param str The format string to use
         * @param ... The arguments to the format string
        */
        void log_always(uint8_t indent, const char* str, ...);//USE THIS SPARINGLY

        /**
         * @brief Check if logging is disabled in this build of libirve
         * @return True if logging is disabled, false otherwise
        */
        bool logging_disabled();
    }

    /**
     * @brief Contains functions to get information about the libirve build
    */
    namespace about {
        /**
         * @brief Get the major version number of libirve
         * @return The major version number of libirve
        */
        std::size_t get_version_major();

        /**
         * @brief Get the minor version number of libirve
         * @return The minor version number of libirve
        */
        std::size_t get_version_minor();

        /**
         * @brief Get the patch version number of libirve
         * @return The patch version number of libirve
        */
        std::size_t get_version_patch();

        /**
         * @brief Get the version string of libirve
         * @return The version string of libirve
        */
        const char* get_version_string();

        /**
         * @brief Get the time of the day that libirve was built
         * @return The time of the day that libirve was built (as a string)
        */
        const char* get_build_time_string();

        /**
         * @brief Get the date that libirve was built on
         * @return The date that libirve was built (as a string)
        */
        const char* get_build_date_string();

        /**
         * @brief Get the build system that libirve was built with
         * @return The build system that libirve was built with (as a string)
        */
        const char* get_build_system_string();

        /**
         * @brief Get the host that libirve was built on
         * @return The host that libirve was built on (as a string)
        */
        const char* get_build_host_string();

        /**
         * @brief Get the target that libirve was built for
         * @return The target that libirve was built for (as a string)
        */
        const char* get_compile_target_string();

        /**
         * @brief Get the compiler that libirve was built with
         * @return The compiler that libirve was built with (as a string)
        */
        const char* get_compiler_string();

        //TODO doxygen
        bool fuzzish_build();
    }

    //Things that depend on previous declarations

    //We have to do it this way to maintain ABI compatibility: https://en.cppreference.com/w/cpp/language/pimpl
    namespace internal::emulator { class emulator_t; }//Forward declaration of the internal class

    /**
     * @brief The namespace containing the actual emulator_t class
    */
    namespace emulator {
        //We have to do it this way to maintain ABI compatibility: https://en.cppreference.com/w/cpp/language/pimpl
        /**
         * @brief The main IRVE emulator class
        */
        class emulator_t {//TODO provide read-only access to the CPU state at the end for integration testing
        public:
            emulator_t() = delete;

            /**
             * @brief Construct a new emulator_t
             * @param imagec The number of images to load into memory
             * @param imagev The names of the images to load into memory (array of char*)
            */
            emulator_t(int imagec, const char* const* imagev);

            /**
             * @brief Destroy an emulator_t and free up its resources
            */
            ~emulator_t();

            /**
             * @brief Emulate one instruction
            */
            bool tick();//Returns true if the emulator should continue running

            /**
             * @brief Repeatedly emulate instructions
             * @param inst_count The value of minstret at which to stop
             * Runs the emulator until the given instruction count is reached or an exit request is made
             * For dynamic linking to libirve, this is more efficient than calling tick() in a loop
             *
            */
            void run_until(uint64_t inst_count);

            /**
             * @brief Run a GDB server on the given port
             * @param port The port to listen on
             *
            */
            void run_gdbserver(uint16_t port);

            /**
             * @brief Get the current instruction count
             * @return minstret
            */
            uint64_t get_inst_count() const;

        private:
            /**
             * @brief The pointer to the internal emulator_t
             *
             * We must do this for ABI compatibility: https://en.cppreference.com/w/cpp/language/pimpl
            */
            irve::internal::emulator::emulator_t* m_emulator_ptr;
        };
    }
}
