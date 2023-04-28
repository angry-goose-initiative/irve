/* logging.h
 * By: John Jekel
 *
 * Logging facilities for
 *
 * Based on code from rv32esim (in C originally)
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
        fprintf(stderr, "@Inst=%lu>\t", inst_num);
    } else {
        fprintf(stderr, "@Init>   \t");
    }

    while (indent--)
        fputc('\t', stderr);

    va_list list;
    assert(str);
    va_start(list, str);
    vfprintf(stderr, str, list);
    va_end(list);
    putc('\n', stderr);
}
