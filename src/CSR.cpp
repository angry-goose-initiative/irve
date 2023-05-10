/* CSR.cpp
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * TODO description
 *
*/

#include "CSR.h"

#include "rvexception.h"

// TODO what should CSRs be initialized to?
CSR_t::CSR_t() : medeleg(false), mideleg(false), mcause(cause_t::IRVE_COLD_RESET_EXCEPTION) {}
