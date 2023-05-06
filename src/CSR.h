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
private:
    reg_t m_CSR[4096];
public:
    CSR_t();
    reg_t r(reg_t csr, privilege_mode_t privilege_mode) const;
    void w(reg_t csr, privilege_mode_t privilege_mode, reg_t data);
    CSR_t& operator=(CSR_t &c) = delete;
};

#endif
