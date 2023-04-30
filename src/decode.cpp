/* decode.c
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Code to decode RISC-V instructions
 *
 * Based on code from rv32esim
*/

/* Includes */

#include "decode.h"

#include <cassert>
#include <cstdint>
#include <string>

#include "common.h"

#define INST_COUNT inst_count
#include "logging.h"

/* Function Implementations */

decoded_inst_t::decoded_inst_t(uint32_t instruction) {
    if (!instruction || (instruction == 0xFFFFFFFF)) {
        this->m_format = INVALID;
        return;
    }

    this->m_opcode = (opcode_t) ((instruction >> 2) & 0b11111);

    switch (this->m_opcode) {
        //R-type
        case OP:
            this->m_format = R_TYPE;
            this->m_funct7 = (instruction >> 25) & 0b1111111;
            this->m_rs2 = (instruction >> 20) & 0b11111;
            this->m_rs1 = (instruction >> 15) & 0b11111;
            this->m_funct3 = (instruction >> 12) & 0b111;
            this->m_rd = (instruction >> 7) & 0b11111;
            break;
        //I-type
        case LOAD:
        case OP_IMM:
        case JALR:
        case SYSTEM:
        case MISC_MEM:
            this->m_format = I_TYPE;
            this->m_imm = SIGN_EXTEND_TO_32(instruction >> 20, 12);
            this->m_rs1 = (instruction >> 15) & 0b11111;
            this->m_funct3 = (instruction >> 12) & 0b111;
            this->m_rd = (instruction >> 7) & 0b11111;
            this->m_funct7 = instruction >> 25;
            break;
        //S-type
        case STORE:
            this->m_format = S_TYPE;
            this->m_imm = SIGN_EXTEND_TO_32(((instruction >> 20) & 0b111111100000) | ((instruction >> 7) & 0b11111), 12);
            this->m_rs2 = (instruction >> 20) & 0b11111;
            this->m_rs1 = (instruction >> 15) & 0b11111;
            this->m_funct3 = (instruction >> 12) & 0b111;
            break;
        //B-type
        case BRANCH:
            this->m_format = B_TYPE;
            this->m_imm = SIGN_EXTEND_TO_32(((instruction >> 19) & 0b1000000000000) | ((instruction << 4) & 0b100000000000) | ((instruction >> 20) & 0b11111100000) | ((instruction >> 7) & 0b11110), 13);
            this->m_rs2 = (instruction >> 20) & 0b11111;
            this->m_rs1 = (instruction >> 15) & 0b11111;
            this->m_funct3 = (instruction >> 12) & 0b111;
            break;
        //U-type
        case LUI:
        case AUIPC:
            this->m_format = U_TYPE;
            this->m_imm = instruction & 0b11111111111111111111000000000000;
            this->m_rd = (instruction >> 7) & 0b11111;
            break;
        //J-type
        case JAL:
            this->m_format = J_TYPE;
            this->m_imm = SIGN_EXTEND_TO_32(((instruction >> 11) & 0b100000000000000000000) | (instruction & 0b11111111000000000000) | ((instruction >> 9) & 0b100000000000) | ((instruction >> 20) & 0b11111111110), 21);
            this->m_rd = (instruction >> 7) & 0b11111;
            break;
        default:
            this->m_format = INVALID;
            break;
    }
}

void decoded_inst_t::log(uint8_t indent, uint64_t inst_count) const {
    switch (this->get_format()) {
        case R_TYPE:
            irvelog(indent, "pretty = %s", this->disassemble().c_str());
            irvelog(indent, "type   = R");
            irvelog(indent, "opcode = 0x%X", this->get_opcode());
            irvelog(indent, "funct3 = 0x%X", this->get_funct3());
            irvelog(indent, "funct7 = 0x%X", this->get_funct7());
            irvelog(indent, "rd     = x%u", this->get_rd());
            irvelog(indent, "rs1    = x%u", this->get_rs1());
            irvelog(indent, "rs2    = x%u", this->get_rs2());
            break;
        case I_TYPE:
            irvelog(indent, "pretty = %s", this->disassemble().c_str());
            irvelog(indent, "type   = I");
            irvelog(indent, "opcode = 0x%X", this->get_opcode());
            irvelog(indent, "funct3 = 0x%X", this->get_funct3());
            irvelog(indent, "rd     = x%u", this->get_rd());
            irvelog(indent, "rs1    = x%u", this->get_rs1());
            irvelog(indent, "imm    = 0x%X", this->get_imm());
            break;
        case S_TYPE:
            irvelog(indent, "pretty = %s", this->disassemble().c_str());
            irvelog(indent, "type   = S");
            irvelog(indent, "opcode = 0x%X", this->get_opcode());
            irvelog(indent, "funct3 = 0x%X", this->get_funct3());
            irvelog(indent, "rs1    = x%u", this->get_rs1());
            irvelog(indent, "rs2    = x%u", this->get_rs2());
            irvelog(indent, "imm    = 0x%X", this->get_imm());
            break;
        case B_TYPE:
            irvelog(indent, "pretty = %s", this->disassemble().c_str());
            irvelog(indent, "type   = B");
            irvelog(indent, "opcode = 0x%X", this->get_opcode());
            irvelog(indent, "funct3 = 0x%X", this->get_funct3());
            irvelog(indent, "rs1    = x%u", this->get_rs1());
            irvelog(indent, "rs2    = x%u", this->get_rs2());
            irvelog(indent, "imm    = 0x%X", this->get_imm());
            break;
        case U_TYPE:
            irvelog(indent, "pretty = %s", this->disassemble().c_str());
            irvelog(indent, "type   = U");
            irvelog(indent, "opcode = 0x%X", this->get_opcode());
            irvelog(indent, "rd     = x%u", this->get_rd());
            irvelog(indent, "imm    = 0x%X", this->get_imm());
            break;
        case J_TYPE:
            irvelog(indent, "pretty = %s", this->disassemble().c_str());
            irvelog(indent, "type   = J");
            irvelog(indent, "opcode = 0x%X", this->get_opcode());
            irvelog(indent, "rd     = x%u", this->get_rd());
            irvelog(indent, "imm    = 0x%X", this->get_imm());
            break;
        case INVALID:
            irvelog(indent, "Invalid Instruction!");
            break;
    }
}

bool decoded_inst_t::is_valid() const {
    return (this->get_format() != INVALID);
}

inst_format_t decoded_inst_t::get_format() const {
    return this->m_format;
}

opcode_t decoded_inst_t::get_opcode() const {
    assert((this->get_format() != INVALID) && "Attempt to get opcode of invalid instruction!");
    return this->m_opcode;
}

uint8_t decoded_inst_t::get_funct3() const {
    assert((this->get_format() != INVALID) && "Attempt to get funct3 of invalid instruction!");
    assert((this->get_format() != U_TYPE) && "Attempt to get funct3 of U-type instruction!");
    assert((this->get_format() != J_TYPE) && "Attempt to get funct3 of J-type instruction!");
    return this->m_funct3;
}

uint8_t decoded_inst_t::get_funct7() const {
    assert((this->get_format() != INVALID) && "Attempt to get funct7 of invalid instruction!");
    assert((this->get_format() == R_TYPE) && "Attempt to get funct7 of non-R-type instruction!");
    return this->m_funct7;
}

uint8_t decoded_inst_t::get_rd() const {
    assert((this->get_format() != INVALID) && "Attempt to get rd of invalid instruction!");
    assert((this->get_format() != S_TYPE) && "Attempt to get rd of S-type instruction!");
    assert((this->get_format() != B_TYPE) && "Attempt to get rd of B-type instruction!");
    return this->m_rd;
}

uint8_t decoded_inst_t::get_rs1() const {
    assert((this->get_format() != INVALID) && "Attempt to get rs1 of invalid instruction!");
    assert((this->get_format() != U_TYPE) && "Attempt to get rs1 of U-type instruction!");
    assert((this->get_format() != J_TYPE) && "Attempt to get rs1 of J-type instruction!");
    return this->m_rs1;
}

uint8_t decoded_inst_t::get_rs2() const {
    assert((this->get_format() != INVALID) && "Attempt to get rs2 of invalid instruction!");
    assert((this->get_format() != I_TYPE) && "Attempt to get rs2 of I-type instruction!");
    assert((this->get_format() != U_TYPE) && "Attempt to get rs2 of U-type instruction!");
    assert((this->get_format() != J_TYPE) && "Attempt to get rs2 of J-type instruction!");
    return this->m_rs2;
}

uint32_t decoded_inst_t::get_imm() const {
    assert((this->get_format() != INVALID) && "Attempt to get imm of invalid instruction!");
    assert((this->get_format() != R_TYPE) && "Attempt to get imm of R-type instruction!");
    return this->m_imm;
}

std::string decoded_inst_t::disassemble() const {
    assert((this->get_format() != INVALID) && "Attempt to disassemble invalid instruction!");

    return std::string("TODO disassembly of instruction here");//TODO
}
