/**
 * @file    rvexception.h
 * @brief   C++ exceptions corresponding to RISC-V exceptions
 * 
 * @copyright Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * This is HORRIBLY inefficient and should be replaced with something better
 * Which is why, when we do XRVE in Rust, we'll use Result<T, E> instead with a custom error enum
 * 
*/

#ifndef RVEXCEPTION_H
#define RVEXCEPTION_H

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include <stdexcept>
#include <cassert>

#include "common.h"

/* ------------------------------------------------------------------------------------------------
 * Constants/Defines
 * --------------------------------------------------------------------------------------------- */

//FIXME add doxygen comments
#define invoke_rv_exception_by_num(the_cause) do {                                                \
    irve::internal::rvexception::cause_t enum_cause =                                             \
        (irve::internal::rvexception::cause_t)(the_cause);                                        \
    switch (enum_cause) {                                                                         \
        case irve::internal::rvexception::cause_t::INSTRUCTION_ADDRESS_MISALIGNED_EXCEPTION:      \
        case irve::internal::rvexception::cause_t::INSTRUCTION_ACCESS_FAULT_EXCEPTION:            \
        case irve::internal::rvexception::cause_t::ILLEGAL_INSTRUCTION_EXCEPTION:                 \
        case irve::internal::rvexception::cause_t::BREAKPOINT_EXCEPTION:                          \
        case irve::internal::rvexception::cause_t::LOAD_ADDRESS_MISALIGNED_EXCEPTION:             \
        case irve::internal::rvexception::cause_t::LOAD_ACCESS_FAULT_EXCEPTION:                   \
        case irve::internal::rvexception::cause_t::STORE_OR_AMO_ADDRESS_MISALIGNED_EXCEPTION:     \
        case irve::internal::rvexception::cause_t::STORE_OR_AMO_ACCESS_FAULT_EXCEPTION:           \
        case irve::internal::rvexception::cause_t::UMODE_ECALL_EXCEPTION:                         \
        case irve::internal::rvexception::cause_t::SMODE_ECALL_EXCEPTION:                         \
        case irve::internal::rvexception::cause_t::MMODE_ECALL_EXCEPTION:                         \
        case irve::internal::rvexception::cause_t::INSTRUCTION_PAGE_FAULT_EXCEPTION:              \
        case irve::internal::rvexception::cause_t::LOAD_PAGE_FAULT_EXCEPTION:                     \
        case irve::internal::rvexception::cause_t::STORE_OR_AMO_PAGE_FAULT_EXCEPTION:             \
            break;                                                                                \
        default:                                                                                  \
            assert(false && "Attempt to invoke exception with unsupported cause!");               \
    }                                                                                             \
    throw irve::internal::rvexception::rvexception_t(enum_cause);                                 \
} while (0)

/**
 * @brief Invoke a RISC-V exception (more concise than using throw)
 * @param the_cause The cause of the exception
*/
#define invoke_rv_exception(the_cause) do {                                                       \
    throw irve::internal::rvexception::rvexception_t(                                             \
        irve::internal::rvexception::cause_t::the_cause ## _EXCEPTION);                           \
} while (0)

/**
 * @brief Invoke an IRVE exit request (more concise than using throw)
*/
#define invoke_polite_irve_exit_request() do {                                                    \
    throw irve::internal::rvexception::irve_exit_request_t();                                     \
} while (0)

/* ------------------------------------------------------------------------------------------------
 * Type/Class Declarations
 * --------------------------------------------------------------------------------------------- */

namespace irve::internal::rvexception {

/* ------------------------------------------------------------------------------------------------
 * Type/Class Declarations
 * --------------------------------------------------------------------------------------------- */

/**
 * @brief contents of the mcause/scause registers, indicating the reason for the
 * interrupt/exception
*/
enum class cause_t : uint32_t {
    // Interrupts
    SUPERVISOR_SOFTWARE_INTERRUPT               = 1     | 0x80000000,
    MACHINE_SOFTWARE_INTERRUPT                  = 3     | 0x80000000,
    SUPERVISOR_TIMER_INTERRUPT                  = 5     | 0x80000000,
    MACHINE_TIMER_INTERRUPT                     = 7     | 0x80000000,
    SUPERVISOR_EXTERNAL_INTERRUPT               = 9     | 0x80000000,
    MACHINE_EXTERNAL_INTERRUPT                  = 11    | 0x80000000,
    // Exceptions
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

/**
 * @brief Exception thrown when an exception is invoked by the RISC-V system
 *
 * Yes throwing exceptions is HORRIBLY inefficient, but it's the simplest path forward when working
 * on our first emulator. We'll stop using exceptions when we do XRVE in Rust. (We'll use Results
 * and the ? operator instead to make things nice and also fast)
*/
class rvexception_t : public std::runtime_error {
public:
    /**
     * @brief Construct a new rvexception_t
     * 
     * @param cause The cause of the interrupt/exception (see cause_t)
    */
    rvexception_t(cause_t cause);
    
    /**
     * @brief Get the cause of the interrupt/exception
     * 
     * @return The cause this exception was constructed with (see cause_t)
    */
    cause_t cause() const;
private:

    /**
     * @brief Cause of the interrupt/exception
    */ 
    cause_t m_cause;
};

/**
 * @brief Exception thrown when the RISC-V system executes IRVE.EXIT
 *
 * Yes throwing exceptions is HORRIBLY inefficient, but it's the simplest path forward when working
 * on our first emulator. We'll stop using exceptions when we do XRVE in Rust. (We'll use Results
 * and the ? operator instead to make things nice and also fast)
*/
class irve_exit_request_t : public std::exception {
public:
    /**
     * @brief Construct a new irve_exit_request_t
    */
    irve_exit_request_t();

    /**
     * @brief Override of std::exception::what()
     * @return The string describing that this exception should always be caught and handled
    */
    const char* what() const noexcept override;
};

}

#endif//RVEXCEPTION_H
