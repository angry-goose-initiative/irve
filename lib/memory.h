/**
 * @brief   Handles the memory of the emulator
 * 
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  Copyright (C) 2023-2024 Nick Chan\n
 *  Copyright (C) 2024 Sam Graham\n
 *  See the LICENSE file at the root of the project for licensing info.
*/

#pragma once

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include <iostream>
#include <memory>

#include "common.h"

#include "aclint.h"
#include "csr.h"
#include "uart.h"

/* ------------------------------------------------------------------------------------------------
 * Type/Class Declarations
 * --------------------------------------------------------------------------------------------- */

namespace irve::internal {

typedef enum {
    AS_OKAY = 0,
    AS_VIOLATES_PMP = 1,
    AS_VIOLATES_PMA = 2,
    AS_MISALIGNED = 3
} access_status_t;

typedef enum {
    IL_OKAY,
    IL_FAIL
} image_load_status_t;

// Facilitates address translation, memory protection, and loading the memory image file
class Memory {
public:

    /**
     * @brief       The constructor when not loading memory image files.
     * @param[in]   CSR_ref A reference to the CSR's.
    */
    Memory(Csr& CSR_ref);

    /**
     * @brief       The constructor when loading memory image files.
     * @param[in]   imagec The number of memory image files to load.
     * @param[in]   imagev Vector of memory image file names.
     * @param[in]   CSR_ref A reference to the CSR's.
    */
    Memory(int imagec, const char* const* imagev, Csr& CSR_ref);

    /**
     * @brief       The destructor.
    */
    ~Memory();

    /**
     * @brief       Fetch an instruction from memory.
     * @note        This can raise exceptions.
     * @param[in]   addr The address to fetch from (physical or virtual depending on operating
     *              mode).
     * @return      The instruction from memory.
    */
    Word instruction(Word addr);

    /**
     * @brief       Load data from memory.
     * @note        This can raise exceptions.
     * @param[in]   addr The address to load from (physical or virtual depending on operating
     *              mode).
     * @param[in]   data_type From funct3 of memory instructions, specifies data width and
     *              signed/unsigned
     * @return      The data read from memory
    */
    Word load(Word addr, uint8_t data_type);

    /**
     * @brief       Store data to memory.
     * @note        This can raise exceptions.
     * @param[in]   addr The address to write to (physical or virtual depending on operating mode).
     * @param[in]   data_type From funct3 of memory instructions, specifies data width and
     *              signed/unsigned.
     * @param[in]   data The data to be stored in memory.
    */
    void store(Word addr, uint8_t data_type, Word data);

    /**
     * @brief       Update peripherals (usually to check if the external interrupt pending bit should be set).
    */
    void update_peripherals();
private:

    /**
     * @brief       Translates a 32 bit address to a 34 bit machine address.
     * @param[in]   untranslated_address 32 bit address.
     * @param[in]   access_type Address translation may raise exceptions for different things
     *              depending on the acces type.
     * @return      34 bit machine address.
    */
    uint64_t translate_address(Word untranslated_addr, uint8_t access_type);

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
    Word read_memory(uint64_t addr, uint8_t data_type, access_status_t& access_status);

    Word read_memory_region_user_ram(uint64_t addr, uint8_t data_type, access_status_t& access_status) const;
    Word read_memory_region_kernel_ram(uint64_t addr, uint8_t data_type, access_status_t& access_status) const;
    Word read_memory_region_aclint(uint64_t addr, uint8_t data_type, access_status_t& access_status);
    Word read_memory_region_uart(uint64_t addr, uint8_t data_type, access_status_t& access_status);

    /**
     * @brief       Write data to memory.
     * @param[in]   addr 34 bit machine address.
     * @param[in]   data_type Specifies data width.
     * @param[in]   data The data to be written to memory.
     * @param[out]  access_status The status of the access (success or not and if not, what the
     *              issue was).
    */
    void write_memory(uint64_t addr, uint8_t data_type, Word data, access_status_t& access_status);

    void write_memory_region_user_ram(uint64_t addr, uint8_t data_type, Word data, access_status_t& access_status);
    void write_memory_region_kernel_ram(uint64_t addr, uint8_t data_type, Word data, access_status_t& access_status);
    void write_memory_region_aclint(uint64_t addr, uint8_t data_type, Word data, access_status_t& access_status);
    void write_memory_region_uart(uint64_t addr, uint8_t data_type, Word data, access_status_t& access_status);
    void write_memory_region_debug(uint64_t addr, uint8_t data_type, Word data, access_status_t& access_status);

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
     * @brief       Loads a 32-bit elf file to memory.
     * @param[in]   image_path The path to the memory image file.
     * @return      Status of the load.
    */
    image_load_status_t load_elf_32(std::string image_path);

    // Reference to the CSRs since memory operations depend on them.
    Csr& m_CSR_ref;

    // Pointer to user ram.
    std::unique_ptr<uint8_t[]> m_user_ram;

    // Pointer to kernel ram.
    std::unique_ptr<uint8_t[]> m_kernel_ram;

    /**
     * @brief       ACLINT
    */
    Aclint m_aclint;

    // 16550 UART.
    Uart m_uart;

    // Output line buffer.
    std::string m_output_line_buffer;
};

} // namespace irve::internal
