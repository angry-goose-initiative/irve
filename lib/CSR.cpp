/* CSR.cpp
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * TODO description
 *
*/

#include "CSR.h"

#include <cstddef>

// TODO what should CSRs be initialized to?
CSR_t::CSR_t() {
    for (std::size_t i = 0; i < 32; ++i) {
        this->medeleg[i] = false;
        this->mideleg[i] = false;
    }
}
