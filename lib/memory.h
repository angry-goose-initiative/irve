/**
 * @brief   Handles the memory of the emulator
 * 
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  Copyright (C) 2023 Nick Chan\n
 *  See the LICENSE file at the root of the project for licensing info.
*/

#pragma once

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include <memory>

#include "common.h"
#include "aclint.h"
#include "csr.h"
#include "uart.h"

/* ------------------------------------------------------------------------------------------------
 * Type/Class Declarations
 * --------------------------------------------------------------------------------------------- */

namespace irve::internal {

// Facilitates address translation, memory protection, and loading the memory image file
class Memory {
public:

    Memory() = delete;

    /**
     * @brief       The constructor.
     * @param[in]   imagec The number of memory image files to load.
     * @param[in]   imagev Vector of memory image file names.
     * @param[in]   CSR_ref A reference to the CSR's.
    */
    Memory(Csr& CSR_ref, int imagec = 0, const char* const* imagev = nullptr);

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

private:

    // Memory access types
    enum class AccessType {
        INSTRUCTION = 0,
        LOAD = 1,
        STORE = 3
    };

    enum class AccessStatus {
        OKAY = 0,
        VIOLATES_PMP = 1,
        VIOLATES_PMA = 2,
        MISALIGNED = 3
    };

    enum class ImageLoadStatus {OKAY, ERROR};

    /**
     * @brief       Translates a 32 bit address to a 34 bit machine address.
     * @param[in]   untranslated_address 32 bit address.
     * @param[in]   access_type Address translation may raise exceptions for different things
     *              depending on the acces type.
     * @return      34 bit machine address.
    */
    uint64_t translate_address(Word untranslated_addr, AccessType access_type);

    /**
     * @brief       Checks if an address should be translated or not.
     * @param[in]   access_type Whether address translation happens or not may depend on whether
     *              the access type is instruction.
     * @return      True for bare translation, false for sv32 translation.
    */
    bool no_address_translation(AccessType access_type) const;

    /**
     * @brief       Read the specified data type from memory.
     * @param[in]   addr 34 bit machine address.
     * @param[in]   data_type Specifies data width and signed/unsigned.
     * @param[out]  access_status The status of the access (success or not and if not, what the
     *              issue was).
     * @return      32 bit version of data that was read.
    */
    Word read_memory(uint64_t addr, uint8_t data_type, AccessStatus& access_status);

    Word read_memory_region_user_ram(uint64_t addr, uint8_t data_type, AccessStatus& access_status) const;
    Word read_memory_region_kernel_ram(uint64_t addr, uint8_t data_type, AccessStatus& access_status) const;
    Word read_memory_region_aclint(uint64_t addr, uint8_t data_type, AccessStatus& access_status);
    Word read_memory_region_uart(uint64_t addr, uint8_t data_type, AccessStatus& access_status);

    /**
     * @brief       Write data to memory.
     * @param[in]   addr 34 bit machine address.
     * @param[in]   data_type Specifies data width.
     * @param[in]   data The data to be written to memory.
     * @param[out]  access_status The status of the access (success or not and if not, what the
     *              issue was).
    */
    void write_memory(uint64_t addr, uint8_t data_type, Word data, AccessStatus& access_status);

    void write_memory_region_user_ram(uint64_t addr, uint8_t data_type, Word data, AccessStatus& access_status);
    void write_memory_region_kernel_ram(uint64_t addr, uint8_t data_type, Word data, AccessStatus& access_status);
    void write_memory_region_aclint(uint64_t addr, uint8_t data_type, Word data, AccessStatus& access_status);
    void write_memory_region_uart(uint64_t addr, uint8_t data_type, Word data, AccessStatus& access_status);
    void write_memory_region_debug(uint64_t addr, uint8_t data_type, Word data, AccessStatus& access_status);

    // TODO(Nick) Move loading functions to separate file

    /**
     * @brief       Loads memory image files (only called by the constructor).
     * @param[in]   imagec The number of memory image files.
     * @param[in]   imagev Vector of memory image file names.
     * @return      Status of the load.
    */
    ImageLoadStatus load_memory_image_files(int imagec, const char* const* imagev);

    /**
     * @brief       Loads a flat binary file to memory.
     * @param[in]   image_path The path to the memory image file.
     * @param[in]   start_addr The address to start loading the image at.
     * @return      Status of the load.
    */
    ImageLoadStatus load_raw_bin(std::string image_path, uint64_t start_addr);

    /**
     * @brief       Loads an 8-bit Verilog hex file to memory.
     * @param[in]   image_path The path to the memory image file.
     * @return      Status of the load
    */
    ImageLoadStatus load_verilog_8(std::string image_path);

    /**
     * @brief       Loads a 32-bit Verilog hex file to memory.
     * @param[in]   image_path The path to the memory image file.
     * @return      Status of the load.
    */
    ImageLoadStatus load_verilog_32(std::string image_path);

    // Reference to the CSRs since memory operations depend on them.
    Csr& csr;

    // Pointer to user ram.
    std::unique_ptr<uint8_t[]> m_user_ram;

    // Pointer to kernel ram.
    std::unique_ptr<uint8_t[]> m_kernel_ram;

    // ACLINT
    Aclint m_aclint;

    // 16550 UART.
    Uart m_uart;

    // Output line buffer.
    std::string m_output_line_buffer;
};

} // namespace irve::internal
