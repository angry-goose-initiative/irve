/* memory.h
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * The classes for the memory of the emulator
 *
 * Manages virtual memory and physical memory
 * 
*/

#ifndef MEMORY_H
#define MEMORY_H

/* Constants And Defines */

//TODO put these into a namespace as regular C++ constants

// The directory where test files are located
#define TESTFILES_DIR   "rvsw/compiled/"

#define BYTE_MASK       0xFF

#define DATA_WIDTH_MASK 0b11

// Emulator memory size is 64 MiB
#define RAMSIZE         0x04000000

// RISC-V code that writes a series of bytes to this address will print them to stdout (flushed
// when a newline is encountered)
#define DEBUGADDR       0xFFFFFFFF

// Virtual address translation

// FIXME move macros to cpp file

// A RISC-V page size is 4 KiB
#define PAGESIZE        0x1000

// Current address translation scheme
// 0 = Bare (no address translation)
// 1 = SV32
#define satp_MODE       m_CSR_ref.implicit_read(CSR::address::SATP).bit(31).u
// The physical page number field of the satp CSR
#define satp_PPN        (uint64_t)m_CSR_ref.implicit_read(CSR::address::SATP).bits(21, 0).u

// Make eXecutable readable field of the mstatus CSR
#define mstatus_MXR     m_CSR_ref.implicit_read(CSR::address::MSTATUS).bit(19).u
// permit Superisor User Memory access field of the mstatus CSR
#define mstatus_SUM     m_CSR_ref.implicit_read(CSR::address::MSTATUS).bit(18).u
// Modify PriVilege field of the mstatus CSR
#define mstatus_MPRV    m_CSR_ref.implicit_read(CSR::address::MSTATUS).bit(17).u
// Pervious privilige mode field of the mstatus CSR
#define mstatus_MPP     m_CSR_ref.implicit_read(CSR::address::MSTATUS).bits(12, 11).u

// The virtual page number (VPN) of a virtual address (va)
#define va_VPN(i)       (uint64_t)va.bits(21 + 10 * i, 12 + 10 * i).u

// Full physical page number field of the page table entry
#define pte_PPN         (uint64_t)pte.bits(31, 10).u
// Upper part of the page number field of the page table entry
#define pte_PPN1        (uint64_t)pte.bits(31, 20).u
// Lower part of the page number field of the page table entry
#define pte_PPN0        pte.bits(19, 10).u
// Page dirty bit
#define pte_D           pte.bit(7).u
// Page accessed bit
#define pte_A           pte.bit(6).u
// Global mapping bit
#define pte_G           pte.bit(5).u
// Page is accessible in U-mode bit
#define pte_U           pte.bit(4).u
// Page is executable bit
#define pte_X           pte.bit(3).u
// Page is writable bit
#define pte_W           pte.bit(2).u
// Page is readable bit
#define pte_R           pte.bit(1).u
// Page table entry valid bit
#define pte_V           pte.bit(0).u

// The current privilege mode
#define CURR_PMODE      m_CSR_ref.get_privilege_mode()

// The conditions for no address translation
#define NO_TRANSLATION  (CURR_PMODE == CSR::privilege_mode_t::MACHINE_MODE) || \
                        ((CURR_PMODE == CSR::privilege_mode_t::SUPERVISOR_MODE) && (satp_MODE == 0))

// The page cannot be accessed if one of the following conditions is met:
                            /* Fetching an instruction but the page is not marked as executable */
#define ACCESS_NOT_ALLOWED  ((access_type == AT_INSTRUCTION) && (pte_X != 1)) || \
                            /* Access is a store but the page is not marked as writable */ \
                            ((access_type == AT_STORE) && (pte_W != 1)) || \
                            /* Access is a load but the page is either not marked as readable or the \
                               page is marked as exectuable but executable pages cannot be read */ \
                            ((access_type == AT_LOAD) && ((pte_R != 1) || ((pte_X == 1) && (mstatus_MXR == 0)))) || \
                            /* Either the current privilege mode is S or the effective privilege mode is S with \
                               the access being a load or a store (not an instruction) and S-mode can't access \
                               U-mode pages and the page is markes as accessible in U-mode */ \
                            (((CURR_PMODE == CSR::privilege_mode_t::SUPERVISOR_MODE) || \
                                ((access_type != AT_INSTRUCTION) && (mstatus_MPP == 0b01) && (mstatus_MPRV == 1))) && \
                                (mstatus_SUM == 0) && (pte_U == 1))

// Access types
#define AT_INSTRUCTION  0
#define AT_LOAD         1
#define AT_STORE        2

#define PAGE_FAULT_BASE 12


/* Includes */

#include <iostream>
#include <memory>

#include "common.h"
#include "CSR.h"

/* Function/Class Declarations */

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
         * @brief Read a byte from memory & throw exception if the byte is not readable
         * @param addr 34 bit machine address
         * @return The byte read
        */
        uint8_t read_byte(uint64_t addr) const;

        /**
         * @brief Write a byte to memory
         * @param addr 34 bit machine address
         * @param data The data to be written
         * This function does NOT raise exceptions if the byte is not writable. To verify that the
         * byte is writable, `check_writable_byte` should always be called first.
        */
        void write_byte(uint64_t addr, uint8_t data);

        /**
         * @brief Throws an exception if the byte is not writable
         * @param addr 34 bit machine address
         * This function should always be used to check that a byte is writable before writing to
         * the byte since `write_byte` assumes the byte is writable. Note that this only checks if
         * the byte is physically writable; privilege level checks and so on are handled elsewhere.
        */
       void check_writable_byte(uint64_t addr);

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
         * @param imagec The number of memory images plus 1 (comes directly from argc in main)
         * @param imagev Vector of image files (comes directly from argv in main)
         * @param CSR_ref A reference to the CSRs
        */
        memory_t(int imagec, const char** imagev, CSR::CSR_t& CSR_ref);

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
        uint64_t translate_address(word_t untranslated_addr, uint8_t access_type) const;

        /**
         * @brief Read the specified data type from memory
         * @param addr 34 bit machine address
         * @param data_type From funct3 of memory instructions, specifies data width and
         *                  signed/unsigned
         * @return 32 bit version of data that was read
        */
        word_t read_physical(uint64_t addr, uint8_t data_type) const;

        /**
         * @brief Write data to memory
         * @param addr 34 bit machine address
         * @param data_type From funct3 of memory instrucitons, specifies data width and
         *                  signed/unsigned
         * @param data The data to be written to memory
        */
        void write_physical(uint64_t addr, uint8_t data_type, word_t data);

        /**
         * @brief Loads memory image files (only called by the constructor)
         * @param imagec The number of memory images plus 1 (comes directly from argc in main)
         * @param imagev Vector of image files (comes directly from argv in main)
        */
        void load_memory_image_files(int imagec, const char** imagev);

        /**
         * @brief Loads a Verilog file to memory
         * @param image_path The path to the memory image file
        */
        void load_verilog_32(std::string image_path);

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
