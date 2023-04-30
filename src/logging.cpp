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

/* Function Implementations */

void irve_log_internal_function_dont_use_this_directly(uint64_t inst_num, uint8_t indent, const char* str, ...) {
    if (inst_num) {
        //fprintf(stderr, "@Inst=%lu>\t", inst_num);
        fprintf(stderr, "\x1b[32m@i=\x1b[95m%lu\x1b[1;34m>\x1b[0m\t\t", inst_num);
    } else {
        fprintf(stderr, "\x1b[32m@init\x1b[1;34m>\x1b[0m\t\t");
    }

    while (indent--) {
        fprintf(stderr, "  ");
    }

    va_list list;
    assert(str);
    va_start(list, str);
    vfprintf(stderr, str, list);
    va_end(list);
    putc('\n', stderr);
}
