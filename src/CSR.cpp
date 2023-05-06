/* CSR.cpp
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * TODO description
 *
*/

#include "CSR.h"

// TODO what should CSRs be initialized to?
CSR_t::CSR_t(): m_CSR() {}

reg_t CSR_t::r(reg_t csr, privilege_mode_t privilege_mode) const {
    // TODO check if CSR can be read from
    if((csr.u & 0b1100000000) > ((uint16_t)(privilege_mode) << 8)) {
        // if not readable, throw exception to be caught?
    }
    return m_CSR[csr.u];
}

void CSR_t::w(reg_t csr, privilege_mode_t privilege_mode, reg_t data) {
    // TODO check if CSR can be written to
    if((csr.u >> 10) == 0b11 || (csr.u & 0b1100000000) > ((uint16_t)(privilege_mode) << 8)) {
        // if not writeable, throw exception to be caught?
    }
    //TODO some CSRs are read only, some are write only, some are read/write
    //Sometimes only PARTS of a CSR are writable or affect other bits
    //We need to check for that and deal with it here

    m_CSR[csr.u] = data;
}
