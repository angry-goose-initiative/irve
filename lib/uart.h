/**
 * @brief   16550 UART implementation
 * 
 * @copyright
 *  Copyright (C) 2024 Seb Atkinson\n
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
#include <condition_variable>
#include "tsqueue.h"
#include <queue>
#include <termios.h>

/* ------------------------------------------------------------------------------------------------
 * Type/Class Declarations
 * --------------------------------------------------------------------------------------------- */

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

    bool interrupt_pending();//More convenient than reading ISR and checking bits

private:

    /**
     * @brief Divisor latch access bit
    */
    bool dlab() const;

    void transmit_thread_function();

    void update_receive();
    struct {
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
    } regs;

    int receive_file_fd;
    std::queue<uint8_t> receive_queue;
    struct termios m_original_receive_file_fd_settings;//To restore terminal changes we made when we're done

    std::thread transmit_thread;//Thread for write operations.
    tsqueue::tsqueue_t<uint8_t> async_transmit_queue;//Queue for async transmits.
    bool kill_transmit_thread = false;
    std::condition_variable transmit_condition_variable;
    std::mutex transmit_mutex;
};

} // namespace irve::internal::uart
