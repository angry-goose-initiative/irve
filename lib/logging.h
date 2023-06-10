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

#if IRVE_INTERNAL_CONFIG_DISABLE_LOGGING

//Compiles down to nothing, but prevents warnings/errors if logging is disabled
#define irvelog(indent, ...) do { \
    ((void)(INST_COUNT)); \
    ((void)(indent)); \
} while (0)

#define irvelog_raw(indent, str) do { \
    ((void)(INST_COUNT)); \
    ((void)(indent)); \
    ((void)(str)); \
} while (0)

#else//Logging is enabled

#define irvelog(indent, ...) do { \
    irvelog_always(indent, __VA_ARGS__); \
} while (0)

#endif

//USE THIS SPARINGLY
//The magic of __VA_OPT__ automatically chooses the more efficient function if there are no variadic arguments
#define irvelog_always(indent, str, ...) do { \
    irve::internal::logging::irvelog_internal_ ## __VA_OPT__(variadic_) ## function_dont_use_this_directly(stderr, INST_COUNT, indent, str __VA_OPT__(,) __VA_ARGS__); \
} while (0)

//SHOULD ONLY BE USED FOR RISC-V OUTPUT
//The magic of __VA_OPT__ automatically chooses the more efficient function if there are no variadic arguments
#define irvelog_always_stdout(indent, str, ...) do { \
    irve::internal::logging::irvelog_internal_ ## __VA_OPT__(variadic_) ## function_dont_use_this_directly(stdout, INST_COUNT, indent, str __VA_OPT__(,) __VA_ARGS__); \
} while (0)

/* Function/Class Declarations */

namespace irve::internal::logging {
    //This is more efficient than the variadic version
    void irvelog_internal_function_dont_use_this_directly(FILE* destination, uint64_t inst_num, uint8_t indent, const char* str);

    //The slower version that supports variadic arguments
    void irvelog_internal_variadic_function_dont_use_this_directly(FILE* destination, uint64_t inst_num, uint8_t indent, const char* str, ...);
}

#endif//LOGGING_H
