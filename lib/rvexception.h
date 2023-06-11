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

#ifndef RVEXCEPTION_H
#define RVEXCEPTION_H

/* Includes */

#include <stdexcept>
#include <cassert>

#include "common.h"

/* Macros */

#define invoke_rv_interrupt_by_num(the_cause) do { \
    irve::internal::rvexception::cause_t enum_cause = (irve::internal::rvexception::cause_t) (the_cause); \
    switch (enum_cause) { \
        case irve::internal::rvexception::cause_t::SUPERVISOR_SOFTWARE_INTERRUPT: \
        case irve::internal::rvexception::cause_t::MACHINE_SOFTWARE_INTERRUPT: \
        case irve::internal::rvexception::cause_t::SUPERVISOR_TIMER_INTERRUPT: \
        case irve::internal::rvexception::cause_t::MACHINE_TIMER_INTERRUPT: \
        case irve::internal::rvexception::cause_t::SUPERVISOR_EXTERNAL_INTERRUPT: \
        case irve::internal::rvexception::cause_t::MACHINE_EXTERNAL_INTERRUPT: \
            break; \
        default: \
            assert(false && "Attempt to invoke interrupt with unsupported cause!"); \
    } \
    throw irve::internal::rvexception::rvinterrupt_t(enum_cause); \
} while (0)

#define invoke_rv_exception_by_num(the_cause) do { \
    irve::internal::rvexception::cause_t enum_cause = (irve::internal::rvexception::cause_t) (the_cause); \
    switch (enum_cause) { \
        case irve::internal::rvexception::cause_t::INSTRUCTION_ADDRESS_MISALIGNED_EXCEPTION: \
        case irve::internal::rvexception::cause_t::INSTRUCTION_ACCESS_FAULT_EXCEPTION: \
        case irve::internal::rvexception::cause_t::ILLEGAL_INSTRUCTION_EXCEPTION: \
        case irve::internal::rvexception::cause_t::BREAKPOINT_EXCEPTION: \
        case irve::internal::rvexception::cause_t::LOAD_ADDRESS_MISALIGNED_EXCEPTION: \
        case irve::internal::rvexception::cause_t::LOAD_ACCESS_FAULT_EXCEPTION: \
        case irve::internal::rvexception::cause_t::STORE_OR_AMO_ADDRESS_MISALIGNED_EXCEPTION: \
        case irve::internal::rvexception::cause_t::STORE_OR_AMO_ACCESS_FAULT_EXCEPTION: \
        case irve::internal::rvexception::cause_t::UMODE_ECALL_EXCEPTION: \
        case irve::internal::rvexception::cause_t::SMODE_ECALL_EXCEPTION: \
        case irve::internal::rvexception::cause_t::MMODE_ECALL_EXCEPTION: \
        case irve::internal::rvexception::cause_t::INSTRUCTION_PAGE_FAULT_EXCEPTION: \
        case irve::internal::rvexception::cause_t::LOAD_PAGE_FAULT_EXCEPTION: \
        case irve::internal::rvexception::cause_t::STORE_OR_AMO_PAGE_FAULT_EXCEPTION: \
            break; \
        default: \
            assert(false && "Attempt to invoke exception with unsupported cause!"); \
    } \
    throw irve::internal::rvexception::rvexception_t(enum_cause); \
} while (0)

#define invoke_rv_interrupt(the_cause) do { \
    throw irve::internal::rvexception::rvinterrupt_t(irve::internal::rvexception::cause_t::the_cause ## _INTERRUPT); \
} while (0)

#define invoke_rv_exception(the_cause) do { \
    throw irve::internal::rvexception::rvexception_t(irve::internal::rvexception::cause_t::the_cause ## _EXCEPTION); \
} while (0)

#define invoke_polite_irve_exit_request() do { \
    throw irve::internal::rvexception::irve_exit_request_t(); \
} while (0)

namespace irve::internal::rvexception {

    /* Types */

    enum class cause_t : uint32_t {
        //Interrupts
        SUPERVISOR_SOFTWARE_INTERRUPT               = 1     | 0x80000000,
        MACHINE_SOFTWARE_INTERRUPT                  = 3     | 0x80000000,
        SUPERVISOR_TIMER_INTERRUPT                  = 5     | 0x80000000,
        MACHINE_TIMER_INTERRUPT                     = 7     | 0x80000000,
        SUPERVISOR_EXTERNAL_INTERRUPT               = 9     | 0x80000000,
        MACHINE_EXTERNAL_INTERRUPT                  = 11    | 0x80000000,
        //Exceptions
        INSTRUCTION_ADDRESS_MISALIGNED_EXCEPTION    = 0,
        INSTRUCTION_ACCESS_FAULT_EXCEPTION          = 1,
        ILLEGAL_INSTRUCTION_EXCEPTION               = 2,
        BREAKPOINT_EXCEPTION                        = 3,
        LOAD_ADDRESS_MISALIGNED_EXCEPTION           = 4,
        LOAD_ACCESS_FAULT_EXCEPTION                 = 5,
        STORE_OR_AMO_ADDRESS_MISALIGNED_EXCEPTION   = 6,
        STORE_OR_AMO_ACCESS_FAULT_EXCEPTION         = 7,
        UMODE_ECALL_EXCEPTION                       = 8,
        SMODE_ECALL_EXCEPTION                       = 9,
        MMODE_ECALL_EXCEPTION                       = 11,
        INSTRUCTION_PAGE_FAULT_EXCEPTION            = 12,
        LOAD_PAGE_FAULT_EXCEPTION                   = 13,
        STORE_OR_AMO_PAGE_FAULT_EXCEPTION           = 15
    };

    /* Function/Class Declarations */

    class rv_base_cpp_exception_t : public std::runtime_error {
    public:
        rv_base_cpp_exception_t(cause_t cause);
        
        cause_t cause() const;
    private:
        cause_t m_cause;
    };

    class rvinterrupt_t : public rv_base_cpp_exception_t {
    public:
        rvinterrupt_t(cause_t cause);
    };

    class rvexception_t : public rv_base_cpp_exception_t {
    public:
        rvexception_t(cause_t cause);
    };

    class irve_exit_request_t : public std::exception {
    public:
        irve_exit_request_t();

        const char* what() const noexcept override;
    };

}

#endif//RVEXCEPTION_H
