/**
 * @brief   Memory image loader functions
 * 
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  Copyright (C) 2023-2024 Nick Chan\n
 *  Copyright (C) 2024 Sam Graham\n
 *  See the LICENSE file at the root of the project for licensing info.
*/

#include "memory.h"

#include <cstring>
#include <fstream>
#include <vector>

#define INST_COUNT 0 // We only log at init
#include "logging.h"

using namespace irve::internal;

image_load_status_t Memory::load_raw_bin(std::string image_path, uint64_t start_addr) {
    //Open the file
    const char* filename = image_path.c_str();
    FILE* file = fopen(filename, "rb");
    if (!file) {
        irvelog(1, "Failed to open memory image file \"%s\"", filename);
        return IL_FAIL;
    }

    //Get the file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);
    if (file_size < 0) {
        irvelog(1, "Failed to get file size");
        fclose(file);
        return IL_FAIL;
    }
    irvelog(1, "Memory image file size is %ld bytes", file_size);

    //Read a file into the emulator byte-by-byte
    //TODO do this more efficiently with fread()
    for (long i = 0; i < file_size; ++i) {
        Word data_byte = fgetc(file);
        uint64_t addr = start_addr + (uint64_t)i;

        access_status_t access_status;
        write_memory(addr, DT_BYTE, data_byte, access_status);
        if (access_status != AS_OKAY) {
            fclose(file);
            return IL_FAIL;
        }
        ++addr;
    }

    //Close the file
    fclose(file);
    return IL_OKAY;
}

image_load_status_t Memory::load_verilog8(std::string image_path) {
    std::fstream fin = std::fstream(image_path);
    if (!fin) {
        return IL_FAIL;
    }

    //Read the file token by token
    uint64_t addr = 0;
    std::string token;
    while (fin >> token) {
        assert((token.length() != 0) && "This should never happen");
        if (token.at(0) == '@') { //`@` indicates a new address (ASSUMING 32-BIT WORDS)
            std::string new_addr_str = token.substr(1);
            assert((new_addr_str.length() == 8) &&
                    "Memory image file is not formatted correctly (bad address)");
            addr = std::stoul(new_addr_str, nullptr, 16);
        }
        else { //New data word (32-bit, could be an instruction or data)
            if (token.length() != 2) {
                irvelog(1, "Memory image file is not formatted correctly (bad data)");
                return IL_FAIL;
            }
            
            //The data word this token represents
            Word data_word = (uint32_t)std::stoul(token, nullptr, 16);

            //Write the data word to memory and increment the address to the next word
            access_status_t access_status;
            write_memory(addr, DT_BYTE, data_word, access_status);
            if(access_status != AS_OKAY) {
                return IL_FAIL;
            }
            ++addr;
        }
    }
    return IL_OKAY;
}

image_load_status_t Memory::load_verilog32(std::string image_path) {
    std::fstream fin = std::fstream(image_path);
    if (!fin) {
        return IL_FAIL;
    }

    //Read the file token by token
    uint64_t addr = 0;
    std::string token;
    while (fin >> token) {
        assert((token.length() != 0) && "This should never happen");
        if (token.at(0) == '@') { //`@` indicates a new address (ASSUMING 32-BIT WORDS)
            std::string new_addr_str = token.substr(1);
            assert((new_addr_str.length() == 8) &&
                    "Memory image file is not formatted correctly (bad address)");
            addr = std::stoul(new_addr_str, nullptr, 16);
            addr *= 4; //This is a word address, not a byte address, so multiply by 4
        }
        else { //New data word (32-bit, could be an instruction or data)
            if (token.length() != 8) {
                irvelog(1, "Warning: 32-bit Verilog image file is not formatted correctly (data "
                           "word is not 8 characters long). This is likely an objcopy bug. "
                           "Continuing anyway with assumed leading zeroes...");
            }
            
            //The data word this token represents
            Word data_word = (uint32_t)std::stoul(token, nullptr, 16);

            //Write the data word to memory and increment the address to the next word
            access_status_t access_status;
            write_memory(addr, DT_WORD, data_word, access_status);
            if (access_status != AS_OKAY) {
                return IL_FAIL;
            }
            addr += 4;
        }
    }
    return IL_OKAY;
}

// Elf header structure layout and details found on Wikipedia.org
// https://en.wikipedia.org/wiki/Executable_and_Linkable_Format
image_load_status_t Memory::load_elf32(std::string image_path) {
    std::ifstream file(image_path, std::ios::binary);
    assert(file.is_open() && "Failed to open file");

    /**
     * We effectively strip the ELF file and only load relevent data into memory by
     *  1) Extracting the file header
     *  2) Navigating to the top of the program header table
     *  3) Identifying all program segments with the "load" flag (PT_LOAD)
     *  4) Navigating to the top of the section header table
     *  5) Identifying all sections with the "program data" flag (SHT_PROGBITS)
     *     that are also contained in load segments
     *  6) Loading only the "program data" inside "load" segments into memory
     *
     * NOTE:
     *  - Program headers refer to program segments
     *  - Section headers refer to program sections
     *  - Program sections are subsets of data in program segments
     */

    // File header contains details about the layout and properties of the file
    struct elf32_file_header {
        uint8_t e_ident[16];
        uint16_t e_type;
        uint16_t e_machine;
        uint32_t e_version;
        uint32_t e_entry;
        uint32_t e_phoff;
        uint32_t e_shoff;
        uint32_t e_flags;
        uint16_t e_ehsize;
        uint16_t e_phentsize;
        uint16_t e_phnum;
        uint16_t e_shentsize;
        uint16_t e_shnum;
        uint16_t e_shstrndx;
    } file_header;

    // Read in file header
    file.read((char*)&file_header, sizeof(file_header));
    assert((!file.bad()) && "Bad read");

    // Validate file header
    const char*     ELF_SIGNATURE = "\177ELF";
    const uint16_t  ELF_FILE_TYPE_EXEC = 2;
    const uint16_t  ELF_MACHINE_RISCV = 0xF3;
    if (memcmp(file_header.e_ident, ELF_SIGNATURE, std::strlen(ELF_SIGNATURE)) != 0 // Not an ELF Signature
        || file_header.e_type != ELF_FILE_TYPE_EXEC // Not an executable file
        || file_header.e_machine != ELF_MACHINE_RISCV // Not a riscv file
    ) {
        return IL_FAIL;
    }

    // Unsupported features
    const uint8_t ELF_ADDRESS_SIZE_32                   = 1;
    const uint8_t ELF_TWOS_COMPLEMENT_AND_LITTLE_ENDIAN = 1;
    assert((file_header.e_ident[4] == ELF_ADDRESS_SIZE_32) && "Not using 32 bit address format");
    assert((file_header.e_ident[5] == ELF_TWOS_COMPLEMENT_AND_LITTLE_ENDIAN) && "Not 2's complement & little endien");

    // Custom data structure used for filtering and loading data
    struct elf32_chunk {
        uint32_t offset;
        uint32_t size;
        uint32_t vaddr;
    };

    // Program headers contain information about segments of data in the file
    struct elf32_program_header {
        uint32_t p_type;
        uint32_t p_offset;
        uint32_t p_vaddr;
        uint32_t p_paddr;
        uint32_t p_filesz;
        uint32_t p_memsz;
        uint32_t p_flags;
        uint32_t p_align;
    } program_header;

    std::vector<elf32_chunk> program_chunks;

    file.seekg(file_header.e_phoff, std::ios::beg);
    // Iterate over program header table to find program load segments
    for (uint16_t i = 0; i < file_header.e_shnum; i++) {
        file.read((char*)&program_header, sizeof(program_header));
        // Cache if program page is of type PT_LOAD
        const uint32_t PT_LOAD = 1;
        if (program_header.p_type == PT_LOAD) {
            program_chunks.push_back({
                program_header.p_offset,
                program_header.p_filesz,
                program_header.p_vaddr
            });
        }
    }

    // Section headers contain information about chunks of data within program segment
    struct elf32_section_header {
        uint32_t sh_name;
        uint32_t sh_type;
        uint32_t sh_flags;
        uint32_t sh_addr;
        uint32_t sh_offset;
        uint32_t sh_size;
        uint32_t sh_link;
        uint32_t sh_info;
        uint32_t sh_addralign;
        uint32_t sh_entsize;
    } section_header;

    std::vector<elf32_chunk> section_chunks;

    file.seekg(file_header.e_shoff, std::ios::beg);
    // Iterate over the section header table to identify program data sections
    for (uint16_t i = 0; i < file_header.e_shnum; i++) {
        file.read((char*)&section_header, sizeof(section_header));
        // Filter out section chunks that shouldn't be loaded into memory
        const uint32_t SHT_PROGBITS = 0x1;
        const uint32_t SHT_INIT_ARRAY = 0xe;
        if (!((section_header.sh_type == SHT_PROGBITS) || (section_header.sh_type == SHT_INIT_ARRAY))) {
            continue;
        }
        // Cache if section is contained in any PT_LOAD program segment
        for (elf32_chunk& chunk : program_chunks) {
            if (
                chunk.offset <= section_header.sh_offset &&
                section_header.sh_offset < (chunk.offset + chunk.size)
            ) {
                section_chunks.push_back({
                    section_header.sh_offset,
                    section_header.sh_size,
                    section_header.sh_addr
                });
                break;
            }
        }
    }

    // Iterate over program data found in load segments
    for (elf32_chunk& chunk : section_chunks) {
        file.seekg(chunk.offset, std::ios::beg);
        uint32_t addr = chunk.vaddr;
        // Load section
        while (addr < chunk.vaddr + chunk.size) {
            uint8_t data_word;
            access_status_t access_status;
            file.read((char*)&data_word, sizeof(data_word));
            // Write data to memory
            write_memory(addr, DT_BYTE, (Word)data_word, access_status);
            if (access_status != AS_OKAY) {
                return IL_FAIL;
            }
            addr += sizeof(data_word);
        }
    }

    return IL_OKAY;
}