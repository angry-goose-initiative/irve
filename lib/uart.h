/**
 * @brief   16550 UART implementation
 * 
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  Copyright (C) 2023 Nick Chan\n
 *  See the LICENSE file at the root of the project for licensing info.
 * 
 * TODO longer description
 *
*/

#pragma once

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include <cstdint>
#include <string>
#include <thread>
#include "tsqueue.h"

namespace irve::internal {

/**
 * @brief The IRVE 16550 UART
*/
class Uart {
public:

    enum class Address : uint8_t {
        RHR = 0b000, // Receive Holding Register
        THR = 0b000, // Transmit Holding Register
        IER = 0b001, // Interrupt Enable Register
        ISR = 0b010, // Interrupt Status Register
        FCR = 0b010, // FIFO Control Register
        LCR = 0b011, // Line Control Register
        MCR = 0b100, // Modem Control Register
        LSR = 0b101, // Line Status Register
        MSR = 0b110, // Modem Status Register
        SPR = 0b111, // Scratch Pad Register

        // When LCR.DLAP == 1
        DLL = 0b000, // Divisor Latch LSB
        DLM = 0b001, // Divisor Latch MSB
        PSD = 0b101, // Prescaler Division
    };

    /**
     * @brief The constructor
    */
    Uart();

    /**
     * @brief The desctructor
    */
    ~Uart();
    
    /**
     * @brief Read from the uart peripheral
     * @param register_address The register to read from (3 bit address)
     * @return The register value
     * @note NOT const since it could pop from the FIFO
    */
    uint8_t read(Address register_address);

    /**
     * @brief Write to uart peripheral
     * @param register_address The register to write to
     * @param data The data to write
    */
    void write(Address register_address, uint8_t data);

    bool interrupt_pending() const;//More convenient than reading ISR and checking bits

private:

    /**
     * @brief Divisor latch access bit
    */
    bool dlab() const;

    void write_thread_function();
    void read_thread_function();


    //No need for rhr and thr since they just go directly to stdin/stdout
    //uint8_t m_ier;//Interrupt Enable Register
    uint8_t m_isr;//Interrupt Status Register
    //uint8_t m_fcr;//FIFO Control Register
    uint8_t m_lcr;//Line Control Register
    //uint8_t m_mcr;//Modem Control Register
    uint8_t m_lsr;//Line Status Register
    //uint8_t m_msr;//Modem Status Register
    uint8_t m_spr;//Scratch Pad Register

    //Note: We expose these registers, but we completely ignore their contents
    //since the serial output is the terminal and there are no real "wires" to
    //run at a particular baud rate
    uint8_t m_dll;//Divisor Latch LSB
    uint8_t m_dlm;//Divisor Latch MSB
    uint8_t m_psd;//Prescaler Division

    std::string m_output_line_buffer;

    //Threads are defined to allow reading and writting to be done in parallel, 
    //as would occur in hardware. 
    std::thread read_thread;//Thread for read operations.
    std::thread write_thread;//Thread for write operations.
    
    tsqueue::tsqueue_t<uint8_t> async_read_queue;//A queue for interfacing
    //with the async read thread.
    
    tsqueue::tsqueue_t<std::tuple<uint8_t, uint8_t>> async_write_queue;
    //A queue for interfacing with the async write thread.
};

} // namespace irve::internal::uart
