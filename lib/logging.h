/* logging.h
 * By: John Jekel
 *
 * Logging facilities for
 *
 * Based on code from rv32esim
*/

#ifndef LOGGING_H
#define LOGGING_H

/* Includes */

#include <stdarg.h>
#include <stdint.h>

#include "config.h"

/* Constants And Defines */

#ifndef INST_COUNT
#error "INST_COUNT must be defined before including logging.h"
#endif

#if IRVE_CONFIG_DISABLE_LOGGING
#define irvelog(...) ((void)0)
#else
#define irvelog(indent, ...) \
    do { \
        irve::internal::logging::irvelog_internal_function_dont_use_this_directly(INST_COUNT, indent, __VA_ARGS__); \
    } while (0)
#endif

/* Function/Class Declarations */

namespace irve::internal::logging {
    void irvelog_internal_function_dont_use_this_directly(uint64_t inst_num, uint8_t indent, const char* str, ...);
    void irvelog_internal_function_dont_use_this_directly(uint64_t inst_num, uint8_t indent, const char* str, va_list list);
}

#endif//LOGGING_H
