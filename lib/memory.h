/**
 * @file    memory.h
 * @brief   Handles the memory of the emulator
 * 
 * @copyright Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 * 
 * TODO longer description
 *
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
     * @brief The constructor
     * @param CSR_ref A reference to the CSRs
    */
    memory_t(CSR::CSR_t& CSR_ref);

    /**
     * @brief The constructor
     * @param imagec The number of memory images
     * @param imagev Vector of image files
     * @param CSR_ref A reference to the CSRs
    */
    memory_t(int imagec, const char* const* imagev, CSR::CSR_t& CSR_ref);

    /**
     * @brief The destructor
    */
    ~memory_t();

    /**
     * @brief Fetch instruction from memory (implicit read)
     * @param addr The address to fetch from (physical or virtual depending on operating mode)
     * @return The instruction from memory
    */
    word_t instruction(word_t addr);

    /**
     * @brief Load data from memory
     * @param addr The address to load from (physical or virtual depending on operating mode)
     * @param data_type From funct3 of memory instructions, specifies data width and
     *                  signed/unsigned
     * @return The data read from memory
    */
    word_t load(word_t addr, uint8_t data_type);

    /**
     * @brief Store data to memory
     * @param addr The address to write to (physical or virtual depending on operating mode)
     * @param data_type From funct3 of memory instructions, specifies data width and
     *                  signed/unsigned
     * @param data The data to be stored in memory
    */
    void store(word_t addr, uint8_t data_type, word_t data);

private:

    /**
     * @brief Translates a 32 bit address to a 34 bit machine address
     * @param untranslated_address 32 bit address
     * @param access_type Address translation may raise exceptions for different things
     *                    depending on the acces type
     * @return 34 bit machine address
    */
    uint64_t translate_address(word_t untranslated_addr, uint8_t access_type);

    /**
     * @brief Checks if an address should be translated or not
     * @param access_type Whether address translation happens or not may depend on whether the
     *                    access type is instruction
     * @return true for bare translation, false for sv32 translation
    */
    bool no_address_translation(uint8_t access_type) const;

    /**
     * @brief Read the specified data type from memory
     * @param addr 34 bit machine address
     * @param data_type Specifies data width and signed/unsigned
     * @return 32 bit version of data that was read
    */
    word_t read_memory(uint64_t addr, uint8_t data_type, access_status_t& access_status);

    // TODO documentation
    word_t read_memory_region_user_ram(uint64_t addr, uint8_t data_type, access_status_t& access_status) const;

    // TODO documentation
    word_t read_memory_region_kernel_ram(uint64_t addr, uint8_t data_type, access_status_t& access_status) const;
    
    // TODO documentation
    word_t read_memory_region_mmcsr(uint64_t addr, uint8_t data_type, access_status_t& access_status) const;

    // TODO documentation
    word_t read_memory_region_uart(uint64_t addr, uint8_t data_type, access_status_t& access_status);

    /**
     * @brief Write data to memory
     * @param addr 34 bit machine address
     * @param data_type Specifies data width
     * @param data The data to be written to memory
    */
    void write_memory(uint64_t addr, uint8_t data_type, word_t data, access_status_t& access_status);

    // TODO documentation
    void write_memory_region_user_ram(uint64_t addr, uint8_t data_type, word_t data, access_status_t& access_status);

    // TODO documentation
    void write_memory_region_kernel_ram(uint64_t addr, uint8_t data_type, word_t data, access_status_t& access_status);
    
    // TODO documentation
    void write_memory_region_mmcsr(uint64_t addr, uint8_t data_type, word_t data, access_status_t& access_status);

    // TODO documentation
    void write_memory_region_uart(uint64_t addr, uint8_t data_type, word_t data, access_status_t& access_status);

    // TODO documentation
    void write_memory_region_debug(uint64_t addr, uint8_t data_type, word_t data, access_status_t& access_status);

    /**
     * @brief Loads memory image files (only called by the constructor)
     * @param imagec The number of memory images plus
     * @param imagev Vector of image files
     * @return status of the load
    */
    image_load_status_t load_memory_image_files(int imagec, const char* const* imagev);

    /**
     * @brief Loads a flat binary file to memory
     * @param image_path The path to the memory image file
     * @param start_addr The address to start loading the image at
     * @return status of the load
    */
    image_load_status_t load_raw_bin(std::string image_path, uint64_t start_addr);

    /**
     * @brief Loads an 8-bit Verilog hex file to memory
     * @param image_path The path to the memory image file
     * @return status of the load
    */
    image_load_status_t load_verilog_8(std::string image_path);

    /**
     * @brief Loads a 32-bit Verilog hex file to memory
     * @param image_path The path to the memory image file
     * @return status of the load
    */
    image_load_status_t load_verilog_32(std::string image_path);

    /**
     * @brief Reference to the CSRs since memory operations depend on them
    */
    CSR::CSR_t& m_CSR_ref;

    /**
     * @brief Pointer to user ram
    */
    std::unique_ptr<uint8_t[]> m_user_ram;

    /**
     * @brief Pointer to kernel ram
    */
    std::unique_ptr<uint8_t[]> m_kernel_ram;

    /**
     * @brief TODO
    */
    uart::uart_t m_uart;

    /**
     * @brief TODO
    */
    std::string m_output_line_buffer;

};

}//NAMESPACE//irve::internal::memory

#endif//MEMORY_H
