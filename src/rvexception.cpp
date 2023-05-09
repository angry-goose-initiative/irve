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

/* Function Implementations */

rv_base_cpp_exception_t::rv_base_cpp_exception_t(cause_t cause) :
    std::runtime_error("Uncaught RISC-V interrupt/exception, you should never see this."),
    m_cause(cause)
{}

cause_t rv_base_cpp_exception_t::cause() const {
    return this->m_cause;
}

rvinterrupt_t::rvinterrupt_t(cause_t cause) : rv_base_cpp_exception_t(cause) {
    assert((((uint32_t)cause) >= 0x80000000) && "Attempt to create rvinterrupt_t with exception cause");
}

rvexception_t::rvexception_t(cause_t cause) : rv_base_cpp_exception_t(cause) {
    assert((((uint32_t)cause) < 0x80000000) && "Attempt to create rvexception_t with interrupt cause");
}
