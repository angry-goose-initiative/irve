/**
 * @file    rvexception.cpp
 * @brief   C++ exceptions corresponding to RISC-V exceptions
 * 
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  Copyright (C) 2023 Nick Chan\n
 *  See the LICENSE file at the root of the project for licensing info.
 * 
 * This is HORRIBLY inefficient and should be replaced with something better
 * Which is why, when we do XRVE in Rust, we'll use Result<T, E> instead with a custom error enum
 * 
*/

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include <stdexcept>
#include <cassert>

#include "rvexception.h"
#undef rvinterrupt_t
#undef rvexception_t

using namespace irve::internal;

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

rvexception::rvexception_t::rvexception_t(rvexception::cause_t cause) :
    std::runtime_error("\x1b[91mUncaught RISC-V exception, you should never see this.\x1b[0m"),
    m_cause(cause)
{
    assert((((uint32_t)cause) < 0x80000000) &&
            "Attempt to create rvexception_t with interrupt cause");
}

rvexception::cause_t rvexception::rvexception_t::cause() const {
    return this->m_cause;
}

rvexception::irve_exit_request_t::irve_exit_request_t() {}

const char* rvexception::irve_exit_request_t::what() const noexcept {
    return "\x1b[91mUncaught IRVE exit request, you should never see this.\x1b[0m";
}
