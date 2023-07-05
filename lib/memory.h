/**
 * @file    memory.h
 * @brief   The classes for the memory of the emulator
 * 
 * @copyright Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 * 
 * Manages virtual memory and physical memory
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
#include "memory_map.h"
#include "CSR.h"

/* ------------------------------------------------------------------------------------------------
 * Constants/Defines
 * --------------------------------------------------------------------------------------------- */

// TODO put these into a namespace as regular C++ constants

// TODO determine which defines should be in the source file instead

#define BYTE_MASK       0xFF

#define DATA_WIDTH_MASK 0b11

// Emulator memory size is 64 MiB
//FIXME move this to config.h
#define RAMSIZE         0x04000000

/* ------------------------------------------------------------------------------------------------
 * Type/Class Declarations
 * --------------------------------------------------------------------------------------------- */

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

namespace irve::internal::memory {

    /**
     * @brief Stores the 'physical' memory and handles memory mapped IO
    */
    class pmemory_t {
    public:

        /**
         * @brief The constructor
        */
        pmemory_t();

        /**
         * @brief The destructor
        */
        ~pmemory_t();

        //TODO if/when we implement PMP, this will need an m_CSR_ref too

        /**
         * @brief Read a byte from memory
         * @param addr 34 bit machine address
         * @param access_status the status of the access
         * @return The byte read
        */
        uint8_t read_byte(uint64_t addr, access_status_t &access_status) const;

        /**
         * @brief Write a byte to memory
         * @param addr 34 bit machine address
         * @param data The data to be written
         * 
         * This function does NOT raise exceptions if the byte is not writable. To verify that the
         * byte is writable, `check_writable_byte` should always be called first.
        */
        void write_byte(uint64_t addr, uint8_t data);

        /**
         * @brief Check if a byte is writable
         * @param addr 34 bit machine address
         * @return the status of the access
         * 
         * This function should always be used to check that a byte is writable before
         * writing to the byte since `write_byte` assumes the byte is writable. Note that this only
         * checks if the byte is physically writable; privilege level checks and so on are handled
         * elsewhere.
        */
        access_status_t check_writable_byte(uint64_t addr);

    private:

        /**
         * @brief Pointer to the ram array
        */
        std::unique_ptr<uint8_t[]> m_ram;

        /**
         * @brief TODO
        */
        std::string m_debugstr;
    };

    /**
     * @brief A wrapper for physical memory
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
         * @brief Fetch instruction from memory (implicit read)
         * @param addr The address to fetch from (physical or virtual depending on operating mode)
         * @return The instruction from memory
        */
        word_t instruction(word_t addr) const;

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
        uint64_t translate_address(word_t untranslated_addr, uint8_t access_type) const;

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
         * @param data_type From funct3 of memory instructions, specifies data width and
         *                  signed/unsigned
         * @return 32 bit version of data that was read
        */
        word_t read_physical(uint64_t addr, uint8_t data_type, access_status_t
                             &access_status) const;

        /**
         * @brief Write data to memory
         * @param addr 34 bit machine address
         * @param data_type From funct3 of memory instrucitons, specifies data width and
         *                  signed/unsigned
         * @param data The data to be written to memory
        */
        void write_physical(uint64_t addr, uint8_t data_type, word_t data,
                            access_status_t &access_status);

        /**
         * @brief Loads memory image files (only called by the constructor)
         * @param imagec The number of memory images plus
         * @param imagev Vector of image files
         * @return status of the load
        */
        image_load_status_t load_memory_image_files(int imagec, const char* const* imagev);

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
         * @brief "physical" memory instance
        */
        pmemory_t m_mem;

        /**
         * @brief Reference to the CSRs since memory operations depend on them
        */
        CSR::CSR_t& m_CSR_ref;
    };

}

#endif//MEMORY_H
