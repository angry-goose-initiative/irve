/* decode.c
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Code to decode RISC-V instructions
 *
 * Based on code from rv32esim
*/

//TODO transition to using word_t and reg_t

/* Includes */

#include "decode.h"

#include <cassert>
#include <cstdint>
#include <string>

#include "common.h"

#include "rvexception.h"

#define INST_COUNT inst_count
#include "logging.h"

/* Function Implementations */

decoded_inst_t::decoded_inst_t(word_t instruction) :
    m_opcode((opcode_t)instruction.bits(6, 2).u),
    m_funct3(instruction.bits(14, 12).u),
    m_funct5(instruction.bits(31, 27).u),
    m_funct7(instruction.bits(31, 25).u),
    m_rd    (instruction.bits(11, 7) .u),
    m_rs1   (instruction.bits(19, 15).u),
    m_rs2   (instruction.bits(24, 20).u),
    m_imm_I (instruction.bits(31, 20).sign_extend_upward_from_bit(11).u),
    m_imm_S (
        (
            (instruction.bits(31, 25) << 5) | 
            instruction.bits (11, 7)
        )
        .sign_extend_upward_from_bit(11).u
    ),
    m_imm_B (
        (
            (instruction.bit (31)       << 12)  |
            (instruction.bit (7)        << 11)  | 
            (instruction.bits(30, 25)   << 5)   | 
            (instruction.bits(11, 8)    << 1)   |
            0b0
        )
        .sign_extend_upward_from_bit(12).u
    ),
    m_imm_U (instruction & 0b11111111111111111111000000000000),//Just zero out the lower 12 bits (keep the upper 20)
    m_imm_J (
        (
            (instruction.bit (31)       << 20)  | 
            (instruction.bits(19, 12)   << 12)  | 
            (instruction.bit (20)       << 11)  | 
            (instruction.bits(30, 21)   << 1)   |
            0b0
        )
        .sign_extend_upward_from_bit(20).u
    )
{
    //These are defined invalid RISC-V instructions
    //In addition, we don't support compressed instructions
    if (!instruction || (instruction == 0xFFFFFFFF) || ((instruction & 0b11) != 0b11)) {
        throw rvexception_t(false, ILLEGAL_INSTRUCTION_EXCEPTION);
    }

    switch (this->m_opcode) {
        //R-type
        case OP:
        case AMO:
            this->m_format = R_TYPE;
            break;
        //I-type
        case LOAD:
        case OP_IMM:
        case JALR:
        case SYSTEM:
        case MISC_MEM:
            this->m_format = I_TYPE;
            break;
        //S-type
        case STORE:
            this->m_format = S_TYPE;
            break;
        //B-type
        case BRANCH:
            this->m_format = B_TYPE;
            break;
        //U-type
        case LUI:
        case AUIPC:
            this->m_format = U_TYPE;
            break;
        //J-type
        case JAL:
            this->m_format = J_TYPE;
            break;
        default:
            throw rvexception_t(false, ILLEGAL_INSTRUCTION_EXCEPTION);
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
        default:
            assert(false && "We should never get here");
            break;
    }
}

inst_format_t decoded_inst_t::get_format() const {
    return this->m_format;
}

opcode_t decoded_inst_t::get_opcode() const {
    return this->m_opcode;
}

uint8_t decoded_inst_t::get_funct3() const {
    assert((this->get_format() != U_TYPE) && "Attempt to get funct3 of U-type instruction!");
    assert((this->get_format() != J_TYPE) && "Attempt to get funct3 of J-type instruction!");
    return this->m_funct3;
}

uint8_t decoded_inst_t::get_funct5() const {
    assert((this->get_format() == R_TYPE) && "Attempt to get funct5 of non-R-type instruction!");
    return this->m_funct5;
}

uint8_t decoded_inst_t::get_funct7() const {
    assert((this->get_format() != S_TYPE) && "Attempt to get funct7 of S-type instruction!");
    assert((this->get_format() != B_TYPE) && "Attempt to get funct7 of B-type instruction!");
    assert((this->get_format() != U_TYPE) && "Attempt to get funct7 of U-type instruction!");
    assert((this->get_format() != J_TYPE) && "Attempt to get funct7 of J-type instruction!");
    //We allow I-type in addition to R-type because it is useful for the immediate shift instructions
    return this->m_funct7;
}

uint8_t decoded_inst_t::get_rd() const {
    assert((this->get_format() != S_TYPE) && "Attempt to get rd of S-type instruction!");
    assert((this->get_format() != B_TYPE) && "Attempt to get rd of B-type instruction!");
    return this->m_rd;
}

uint8_t decoded_inst_t::get_rs1() const {
    assert((this->get_format() != U_TYPE) && "Attempt to get rs1 of U-type instruction!");
    assert((this->get_format() != J_TYPE) && "Attempt to get rs1 of J-type instruction!");
    return this->m_rs1;
}

uint8_t decoded_inst_t::get_rs2() const {
    assert((this->get_format() != I_TYPE) && "Attempt to get rs2 of I-type instruction!");
    assert((this->get_format() != U_TYPE) && "Attempt to get rs2 of U-type instruction!");
    assert((this->get_format() != J_TYPE) && "Attempt to get rs2 of J-type instruction!");
    return this->m_rs2;
}

word_t decoded_inst_t::get_imm() const {
    switch (this->get_format()) {
        case R_TYPE:
            assert(false && "Attempt to get imm of R-type instruction!");
            break;
        case I_TYPE:
            return this->m_imm_I;
            break;
        case S_TYPE:
            return this->m_imm_S;
            break;
        case B_TYPE:
            return this->m_imm_B;
            break;
        case U_TYPE:
            return this->m_imm_U;
            break;
        case J_TYPE:
            return this->m_imm_J;
            break;
        default:
            assert(false && "We should never get here");
            break;
    }
    return this->m_imm_I;
}

std::string decoded_inst_t::disassemble() const {
    return std::string("TODO disassembly of instruction here (Save this for XRVE actually)");//TODO
}
