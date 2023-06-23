/* logging.h
 * By: John Jekel
 *
 * Logging facilities for irve
 *
 * Based on code from rv32esim
*/

#ifndef LOGGING_H
#define LOGGING_H

/* Includes */

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#include "config.h"

/* Constants And Defines */

#ifndef INST_COUNT
#error "INST_COUNT must be defined before including logging.h"
#endif

/** \def irvelog(indent, ...)
 * @brief Logs a message to stderr when logging is enabled
 * @param indent The indentation level of the message
 * @param ... The message to log, and any additional format arguments
*/

/** \def irvelog_always(indent, ...)
 * @brief Logs a message to stderr, regardless of whether logging is enabled or not. USE THIS SPARINGLY
 * @param indent The indentation level of the message
 * @param ... The message to log, and any additional format arguments
 *
 * The magic of __VA_OPT__ automatically chooses the more efficient function if there are no variadic arguments
*/

/** \def irvelog_always_stdout(indent, ...)
 * @brief Logs a message to stdout, regardless of whether logging is enabled or not. SHOULD ONLY BE USED FOR RISC-V OUTPUT
 * @param indent The indentation level of the message
 * @param ... The message to log, and any additional format arguments
 *
 * The magic of __VA_OPT__ automatically chooses the more efficient function if there are no variadic arguments
*/

#if IRVE_INTERNAL_CONFIG_DISABLE_LOGGING

//Compiles down to nothing, but prevents warnings/errors if logging is disabled
#define irvelog(indent, ...) do { \
    ((void)(INST_COUNT)); \
    ((void)(indent)); \
} while (0)

#else//Logging is enabled

#define irvelog(indent, ...) do { \
    irvelog_always(indent, __VA_ARGS__); \
} while (0)

#endif

#ifndef _MSC_VER//Not on MSVC

#define irvelog_always(indent, str, ...) do { \
    irve::internal::logging::irvelog_internal_ ## __VA_OPT__(variadic_) ## function_dont_use_this_directly(stderr, INST_COUNT, indent, str __VA_OPT__(,) __VA_ARGS__); \
} while (0)

#define irvelog_always_stdout(indent, str, ...) do { \
    irve::internal::logging::irvelog_internal_ ## __VA_OPT__(variadic_) ## function_dont_use_this_directly(stdout, INST_COUNT, indent, str __VA_OPT__(,) __VA_ARGS__); \
} while (0)

#else//On MSVC

//MSVC doesn't support __VA_OPT__ yet, so we have to use the inefficient logging function always

#define irvelog_always(indent, str, ...) do { \
    irve::internal::logging::irvelog_internal_function_dont_use_this_directly(stderr, INST_COUNT, indent, str, __VA_ARGS__); \
} while (0)

#define irvelog_always_stdout(indent, str, ...) do { \
    irve::internal::logging::irvelog_internal_function_dont_use_this_directly(stdout, INST_COUNT, indent, str, __VA_ARGS__); \
} while (0)

#endif

/* Function/Class Declarations */

namespace irve::internal::logging {
    /**
     * @brief Don't use this function directly. Use the irvelog family of macros instead.
     * @param destination The file to log to
     * @param inst_num The current instruction number
     * @param indent The indentation level of the message
     * @param str The message to log
     *
     * This is more efficient than the variadic version
    */
    void irvelog_internal_function_dont_use_this_directly(FILE* destination, uint64_t inst_num, uint8_t indent, const char* str);

    /**
     * @brief Don't use this function directly. Use the irvelog family of macros instead.
     * @param destination The file to log to
     * @param inst_num The current instruction number
     * @param indent The indentation level of the message
     * @param str The message to log
     * @param ... Any additional format arguments (if any)
     *
     * The slower version that supports variadic arguments
    */
    void irvelog_internal_variadic_function_dont_use_this_directly(FILE* destination, uint64_t inst_num, uint8_t indent, const char* str, ...);
}

#endif//LOGGING_H
