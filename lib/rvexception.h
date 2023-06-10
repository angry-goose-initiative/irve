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

#include "common.h"

/* Macros */

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

    ///Possible contents of the mcause/scause registers, indicating the reason for the interrupt/exception
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

    /**
     * @brief Base class for all RISC-V interrupts and exceptions
     *
     * Yes throwing excpetions is HORRIBLY inefficient, but it's the simplest path forward when working on our first emulator.
     * We'll stop using exceptions when we do XRVE in Rust. (We'll use Results and the ? operator instead to make things nice and also fast)
    */
    class rv_base_cpp_exception_t : public std::runtime_error {
    public:
        /**
         * @brief Construct a new rv_base_cpp_exception_t
         * 
         * @param cause The cause of the interrupt/exception (see cause_t)
        */
        rv_base_cpp_exception_t(cause_t cause);
        
        /**
         * @brief Get the cause of the interrupt/exception
         * 
         * @return The cause this exception was constructed with (see cause_t)
        */
        cause_t cause() const;

        /**
         * @brief Override of std::exception::what()
         * 
         * @return The string describing that this exception should always be caught and handled
        */
        const char* what() const noexcept override;
    private:

        ///The cause of the interrupt/exception
        cause_t m_cause;
    };

    /**
     * @brief Exception thrown when an interrupt is invoked by the RISC-V system
     *
     * Yes throwing exceptions is HORRIBLY inefficient, but it's the simplest path forward when working on our first emulator.
     * We'll stop using exceptions when we do XRVE in Rust. (We'll use Results and the ? operator instead to make things nice and also fast)
    */
    class rvinterrupt_t : public rv_base_cpp_exception_t {
    public:
        /**
         * @brief Construct a new rvinterrupt_t
         * 
         * @param cause The cause of the interrupt/exception (see cause_t)
        */
        rvinterrupt_t(cause_t cause);
    };

    /**
     * @brief Exception thrown when an exception is invoked by the RISC-V system
     *
     * Yes throwing exceptions is HORRIBLY inefficient, but it's the simplest path forward when working on our first emulator.
     * We'll stop using exceptions when we do XRVE in Rust. (We'll use Results and the ? operator instead to make things nice and also fast)
    */
    class rvexception_t : public rv_base_cpp_exception_t {
    public:
        /**
         * @brief Construct a new rvexception_t
         * 
         * @param cause The cause of the interrupt/exception (see cause_t)
        */
        rvexception_t(cause_t cause);
    };

    /**
     * @brief Exception thrown when the RISC-V system executes IRVE.EXIT
     *
     * Yes throwing exceptions is HORRIBLY inefficient, but it's the simplest path forward when working on our first emulator.
     * We'll stop using exceptions when we do XRVE in Rust. (We'll use Results and the ? operator instead to make things nice and also fast)
    */
    class irve_exit_request_t : public std::exception {
    public:
        /**
         * @brief Construct a new irve_exit_request_t
        */
        irve_exit_request_t();

        /**
         * @brief Override of std::exception::what()
         * 
         * @return The string describing that this exception should always be caught and handled
        */
        const char* what() const noexcept override;
    };

}

#endif//RVEXCEPTION_H
