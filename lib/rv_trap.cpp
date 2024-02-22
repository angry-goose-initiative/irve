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
#include <cstdint>

#include "rv_trap.h"
#undef rvinterrupt_t
#undef RvException

using namespace irve::internal;

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

rv_trap::RvException::RvException(rv_trap::Cause cause, Word tval) :
    std::runtime_error("\x1b[91mUncaught RISC-V exception, you should never see this.\x1b[0m"),
    m_cause(cause),
    m_tval(tval)
{
    assert((((uint32_t)cause) < 0x80000000) &&
            "Attempt to create RvException with interrupt cause");
}

rv_trap::Cause rv_trap::RvException::cause() const {
    return this->m_cause;
}

Word rv_trap::RvException::tval() const {
    return this->m_tval;
}

rv_trap::IrveExitRequest::IrveExitRequest() {}

const char* rv_trap::IrveExitRequest::what() const noexcept {
    return "\x1b[91mUncaught IRVE exit request, you should never see this.\x1b[0m";
}
