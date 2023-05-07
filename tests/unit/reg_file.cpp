/* reg_file.cpp
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Performs unit tests for IRVE's reg_file.h and reg_file.cpp
 *
*/

/* Includes */

#include <cassert>
#include "reg_file.h"

/* Function Implementations */

int test_reg_file_t() {
    reg_file_t reg_file;

    assert(reg_file[0] == 0);
    reg_file[0] = 0x12345678;
    assert(reg_file[0] == 0);
    reg_file[1] = 0x98765432;
    assert(reg_file[1] == 0x98765432);
    reg_file[2] = 0xABCD1234;
    assert(reg_file[2] == 0xABCD1234);
    reg_file[3] = 0x1234ABCD;
    assert(reg_file[3] == 0x1234ABCD);
    reg_file[4] = 0x00000000;
    assert(reg_file[4] == 0x00000000);
    reg_file[5] = 0xFFFFFFFF;
    assert(reg_file[5] == 0xFFFFFFFF);
    reg_file[20] = -123;
    assert(reg_file[20] == -123);
    reg_file[31] = 0xFFFFFFFF;
    assert(reg_file[31] == -1);
    assert(reg_file[2] == 0xABCD1234);
    reg_file[2] = 0x7FFFFFFF;
    assert(reg_file[2] == 0x7FFFFFFF);

    //TODO test the const version of operator[]

    return 0;
}
