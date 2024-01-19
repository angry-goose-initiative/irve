/**
 * @file    memory.h
 * @brief   Handles the memory of the emulator
 * 
<<<<<<< HEAD
 * @copyright Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
=======
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  Copyright (C) 2023 Nick Chan\n
 *  See the LICENSE file at the root of the project for licensing info.
 * 
 * TODO longer description
 *
>>>>>>> aa8aacf (Update IRVE Copyright information)
*/

#ifndef MEMORY_H
#define MEMORY_H

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include <iostream>
#include <memory>

#include "common.h"
#include "CSR.h"
#include "uart.h"

/* ------------------------------------------------------------------------------------------------
 * Type/Class Declarations
 * --------------------------------------------------------------------------------------------- */

namespace irve::internal::memory {

/**
 * @brief TODO
*/
typedef enum {
    AS_OKAY = 0,
    AS_VIOLATES_PMP = 1,
    AS_VIOLATES_PMA = 2,
    AS_MISALIGNED = 3
} access_status_t;

/**
 * @brief TODO
*/
typedef enum {
    IL_OKAY,
    IL_FAIL
} image_load_status_t;

/**
 * @brief TODO
 * 
 * Facilitates address translation, memory protection, and loading the memory image file
*/
class memory_t {
public:

    /**
     * @brief       The constructor when not loading memory image files.
     * @param[in]   CSR_ref A reference to the CSR's.
    */
    memory_t(CSR::CSR_t& CSR_ref);

    /**
     * @brief       The constructor when loading memory image files.
     * @param[in]   imagec The number of memory image files to load.
     * @param[in]   imagev Vector of memory image file names.
     * @param[in]   CSR_ref A reference to the CSR's.
    */
    memory_t(int imagec, const char* const* imagev, CSR::CSR_t& CSR_ref);

    /**
     * @brief       The destructor.
    */
    ~memory_t();

    /**
     * @brief       Fetch an instruction from memory.
     * @note        This can raise exceptions.
     * @param[in]   addr The address to fetch from (physical or virtual depending on operating
     *              mode).
     * @return      The instruction from memory.
    */
    word_t instruction(word_t addr);

    /**
     * @brief       Load data from memory.
     * @note        This can raise exceptions.
     * @param[in]   addr The address to load from (physical or virtual depending on operating
     *              mode).
     * @param[in]   data_type From funct3 of memory instructions, specifies data width and
     *              signed/unsigned
     * @return      The data read from memory
    */
    word_t load(word_t addr, uint8_t data_type);

    /**
     * @brief       Store data to memory.
     * @note        This can raise exceptions.
     * @param[in]   addr The address to write to (physical or virtual depending on operating mode).
     * @param[in]   data_type From funct3 of memory instructions, specifies data width and
     *              signed/unsigned.
     * @param[in]   data The data to be stored in memory.
    */
    void store(word_t addr, uint8_t data_type, word_t data);

private:

    /**
     * @brief       Translates a 32 bit address to a 34 bit machine address.
     * @param[in]   untranslated_address 32 bit address.
     * @param[in]   access_type Address translation may raise exceptions for different things
     *              depending on the acces type.
     * @return      34 bit machine address.
    */
    uint64_t translate_address(word_t untranslated_addr, uint8_t access_type);

    /**
     * @brief       Checks if an address should be translated or not.
     * @param[in]   access_type Whether address translation happens or not may depend on whether
     *              the access type is instruction.
     * @return      True for bare translation, false for sv32 translation.
    */
    bool no_address_translation(uint8_t access_type) const;

    /**
     * @brief       Read the specified data type from memory.
     * @param[in]   addr 34 bit machine address.
     * @param[in]   data_type Specifies data width and signed/unsigned.
     * @param[out]  access_status The status of the access (success or not and if not, what the
     *              issue was).
     * @return      32 bit version of data that was read.
    */
    word_t read_memory(uint64_t addr, uint8_t data_type, access_status_t& access_status);

    word_t read_memory_region_user_ram(uint64_t addr, uint8_t data_type, access_status_t& access_status) const;
    word_t read_memory_region_kernel_ram(uint64_t addr, uint8_t data_type, access_status_t& access_status) const;
    word_t read_memory_region_mmcsr(uint64_t addr, uint8_t data_type, access_status_t& access_status) const;
    word_t read_memory_region_uart(uint64_t addr, uint8_t data_type, access_status_t& access_status);

    /**
     * @brief       Write data to memory.
     * @param[in]   addr 34 bit machine address.
     * @param[in]   data_type Specifies data width.
     * @param[in]   data The data to be written to memory.
     * @param[out]  access_status The status of the access (success or not and if not, what the
     *              issue was).
    */
    void write_memory(uint64_t addr, uint8_t data_type, word_t data, access_status_t& access_status);

    void write_memory_region_user_ram(uint64_t addr, uint8_t data_type, word_t data, access_status_t& access_status);
    void write_memory_region_kernel_ram(uint64_t addr, uint8_t data_type, word_t data, access_status_t& access_status);
    void write_memory_region_mmcsr(uint64_t addr, uint8_t data_type, word_t data, access_status_t& access_status);
    void write_memory_region_uart(uint64_t addr, uint8_t data_type, word_t data, access_status_t& access_status);
    void write_memory_region_debug(uint64_t addr, uint8_t data_type, word_t data, access_status_t& access_status);

    /**
     * @brief       Loads memory image files (only called by the constructor).
     * @param[in]   imagec The number of memory image files.
     * @param[in]   imagev Vector of memory image file names.
     * @return      Status of the load.
    */
    image_load_status_t load_memory_image_files(int imagec, const char* const* imagev);

    /**
     * @brief       Loads a flat binary file to memory.
     * @param[in]   image_path The path to the memory image file.
     * @param[in]   start_addr The address to start loading the image at.
     * @return      Status of the load.
    */
    image_load_status_t load_raw_bin(std::string image_path, uint64_t start_addr);

    /**
     * @brief       Loads an 8-bit Verilog hex file to memory.
     * @param[in]   image_path The path to the memory image file.
     * @return      Status of the load
    */
    image_load_status_t load_verilog_8(std::string image_path);

    /**
     * @brief       Loads a 32-bit Verilog hex file to memory.
     * @param[in]   image_path The path to the memory image file.
     * @return      Status of the load.
    */
    image_load_status_t load_verilog_32(std::string image_path);

    /**
     * @brief       Reference to the CSRs since memory operations depend on them.
    */
    CSR::CSR_t& m_CSR_ref;

    /**
     * @brief       Pointer to user ram.
    */
    std::unique_ptr<uint8_t[]> m_user_ram;

    /**
     * @brief       Pointer to kernel ram.
    */
    std::unique_ptr<uint8_t[]> m_kernel_ram;

    /**
     * @brief       16550 UART.
    */
    uart::uart_t m_uart;

    /**
     * @brief       Output line buffer.
    */
    std::string m_output_line_buffer;

};

}//NAMESPACE//irve::internal::memory

#endif//MEMORY_H
