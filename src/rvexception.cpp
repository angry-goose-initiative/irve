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

#include <stdexcept>
#include <string>

#include "rvexception.h"

/* Function Implementations */

rvexception_t::rvexception_t(bool is_interrupt, cause_t cause) :
    std::runtime_error(std::string("Uncaught RISC-V exception, you should never see this.")),
    m_is_interrupt(is_interrupt),
    m_cause(cause)
{}

bool rvexception_t::is_interrupt() const {
    return this->m_is_interrupt;
}

cause_t rvexception_t::cause() const {
    return this->m_cause;
}

word_t rvexception_t::raw_cause() const {
    if (this->m_is_interrupt) {
        return this->m_cause | 0x80000000;
    } else {
        return this->m_cause;
    }
}
