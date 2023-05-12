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

/* Function Implementations */

int test_CSR_CSR_t_init() {
    CSR_t csr;

    for (std::size_t i = 0; i < 32; ++i) {
        assert(csr.medeleg[i] == false);
        assert(csr.mideleg[i] == false);
    }

    //TODO other init tests
    return 0;
}
