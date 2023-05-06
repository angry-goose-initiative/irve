/* CSR.h
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * TODO description
 *
*/

#ifndef CSR_H
#define CSR_H

#include <cstdint>

#include "reg.h"

typedef enum {
    USER_MODE = 0b00,
    SUPERVISOR_MODE = 0b01,
    MACHINE_MODE = 0b11
} privilege_mode_t;

class CSR_t {
public:
    CSR_t();
    //TODO add CSRs HERE
};

#endif
