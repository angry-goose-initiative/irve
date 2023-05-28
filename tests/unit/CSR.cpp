/* CSR.cpp
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Performs unit tests for IRVE's CSR.h and CSR.cpp
 *
*/

/* Includes */

#include <cassert>
#include <cstddef>
#include "common.h"
#include "CSR.h"

using namespace irve::internal;

/* Function Implementations */

int test_CSR_CSR_t_init() {
    CSR::CSR_t csr;

    //Actually, according to the spec, medeleg/mideleg initial state is undefined
    //assert(csr.explicit_read(CSR::address::MEDELEG) == 0);
    //assert(csr.explicit_read(CSR::address::MIDELEG) == 0);
    //assert(csr.implicit_read(CSR::address::MEDELEG) == 0);
    //assert(csr.implicit_read(CSR::address::MIDELEG) == 0);

    //TODO other init tests
    return 0;
}
