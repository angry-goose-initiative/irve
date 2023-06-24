/* rvexception.h
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * C++ exceptions corresponding to RISC-V exceptions
 *
 * This is HORRIBLY inefficient and should be replaced with something better
 * Which is why, when we do XRVE in Rust, we'll use Result<T, E> instead with a custom error enum
 *
*/

/* Includes */

#include <cassert>
#include <stdexcept>

#include "rvexception.h"
#undef rvinterrupt_t
#undef rvexception_t

using namespace irve::internal;

/* Function Implementations */

rvexception::rvexception_t::rvexception_t(rvexception::cause_t cause) :
    std::runtime_error("Uncaught RISC-V interrupt/exception, you should never see this."),
    m_cause(cause)
{
    assert((((uint32_t)cause) < 0x80000000) && "Attempt to create rvexception_t with interrupt cause");
}

rvexception::cause_t rvexception::rvexception_t::cause() const {
    return this->m_cause;
}

rvexception::irve_exit_request_t::irve_exit_request_t() {}

const char* rvexception::irve_exit_request_t::what() const noexcept {
    return "\x1b[91mUncaught IRVE exit request, you should never see this.\x1b[0m";
}
