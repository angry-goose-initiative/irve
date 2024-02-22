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

#pragma once

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include <stdexcept>
#include <cassert>
#include <cstdint>

#include "common.h"

/* ------------------------------------------------------------------------------------------------
 * Type/Class Declarations
 * --------------------------------------------------------------------------------------------- */

namespace irve::internal::rv_trap {

/* ------------------------------------------------------------------------------------------------
 * Type/Class Declarations
 * --------------------------------------------------------------------------------------------- */

/**
 * @brief contents of the mcause/scause registers, indicating the cause of the
 * interrupt/exception
*/
enum class Cause : uint32_t {
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
class RvException : public std::runtime_error {
public:
    /**
     * @brief Construct a new RvException
     * 
     * @param cause The cause of the interrupt/exception (see cause_t)
     * @param tval Extra exception info (the value for mtval or stval)
    */
    RvException(Cause cause, Word tval);
    
    /**
     * @brief Get the cause of the interrupt/exception
     * 
     * @return The cause this exception was constructed with (see cause_t)
    */
    Cause cause() const;

    /**
     * @brief Get the value of mtval or stval for this exception
     * 
     * @return The value of mtval or stval for this exception
    */
    Word tval() const;
private:

    /**
     * @brief Cause of the interrupt/exception
    */ 
    Cause m_cause;

    /**
     * @brief Extra exception info (the value for mtval or stval)
    */ 
    Word m_tval;
};

/**
 * @brief Exception thrown when the RISC-V system executes IRVE.EXIT
 *
 * Yes throwing exceptions is HORRIBLY inefficient, but it's the simplest path forward when working
 * on our first emulator. We'll stop using exceptions when we do XRVE in Rust. (We'll use Results
 * and the ? operator instead to make things nice and also fast)
*/
class IrveExitRequest : public std::exception {
public:
    /**
     * @brief Construct a new IrveExitRequest
    */
    IrveExitRequest();

    /**
     * @brief Override of std::exception::what()
     * @return The string describing that this exception should always be caught and handled
    */
    const char* what() const noexcept override;
};

inline void invoke_exception(Cause cause, Word tval = 0) {
    throw RvException(cause, tval);
}

inline void invoke_polite_irve_exit_request() {
    throw irve::internal::rv_trap::IrveExitRequest();
}

} // namespace irve::internal::rv_trap
