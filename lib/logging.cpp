/* logging.h
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Logging facilities for irve
 *
 * Based on code from rv32esim
*/

/* Includes */

#define INST_COUNT this does not actually need to be defined with anything important before including logging.h in this case
#include "logging.h"

#include <cassert>
#include <cstdarg>
#include <cstdio>

using namespace irve::internal;

/* Function Implementations */

void logging::irvelog_internal_function_dont_use_this_directly(uint64_t inst_num, uint8_t indent, const char* str, ...) {
    va_list list;
    va_start(list, str);
    irvelog_internal_function_dont_use_this_directly(inst_num, indent, str, list);
}

void logging::irvelog_internal_function_dont_use_this_directly(uint64_t inst_num, uint8_t indent, const char* str, va_list list) {
    assert(str && "irvelog called with null format string");

    if (inst_num) {
        fprintf(stderr, "\x1b[94m%-lu\x1b[1;90m>\x1b[0m ", inst_num);
    } else {
        fprintf(stderr, "\x1b[94mStartup\x1b[1;90m>\x1b[0m ");
    }

    while (indent--) {
        fprintf(stderr, "  ");
    }

    vfprintf(stderr, str, list);
    putc('\n', stderr);
}
