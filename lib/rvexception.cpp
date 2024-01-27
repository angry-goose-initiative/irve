/**
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

rv_trap::rvexception_t::rvexception_t(rv_trap::Cause cause) :
    std::runtime_error("\x1b[91mUncaught RISC-V exception, you should never see this.\x1b[0m"),
    m_cause(cause)
{
    assert((((uint32_t)cause) < 0x80000000) &&
            "Attempt to create rvexception_t with interrupt cause");
}

rv_trap::Cause rv_trap::rvexception_t::cause() const {
    return this->m_cause;
}

rv_trap::irve_exit_request_t::irve_exit_request_t() {}

const char* rv_trap::irve_exit_request_t::what() const noexcept {
    return "\x1b[91mUncaught IRVE exit request, you should never see this.\x1b[0m";
}
