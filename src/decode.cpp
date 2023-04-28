/* decode.c
 * By: John Jekel
 *
 * Code to decode RISC-V instructions
 *
 * Based on code from rv32esim
*/

/* Includes */

#include "decode.h"

#include <cassert>
#include <cstdint>

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
            this->m_rs2 = (instruction >> 20) & 0b1111;
            this->m_rs1 = (instruction >> 15) & 0b1111;
            this->m_funct3 = (instruction >> 12) & 0b111;
            this->m_rd = (instruction >> 7) & 0b1111;
            break;
        //I-type
        case LOAD:
        case OP_IMM:
        case JALR:
        case SYSTEM:
            this->m_format = I_TYPE;
            this->m_imm = SIGN_EXTEND_TO_32(instruction >> 20, 12);
            this->m_rs1 = (instruction >> 15) & 0b1111;
            this->m_funct3 = (instruction >> 12) & 0b111;
            this->m_rd = (instruction >> 7) & 0b1111;
            this->m_funct7 = instruction >> 25;
            break;
        //S-type
        case STORE:
            this->m_format = S_TYPE;
            this->m_imm = SIGN_EXTEND_TO_32(((instruction >> 20) & 0b111111100000) | ((instruction >> 7) & 0b11111), 12);
            this->m_rs2 = (instruction >> 20) & 0b1111;
            this->m_rs1 = (instruction >> 15) & 0b1111;
            this->m_funct3 = (instruction >> 12) & 0b111;
            break;
        //B-type
        case BRANCH:
            this->m_format = B_TYPE;
            this->m_imm = SIGN_EXTEND_TO_32(((instruction >> 19) & 0b1000000000000) | ((instruction << 4) & 0b100000000000) | ((instruction >> 20) & 0b11111100000) | ((instruction >> 7) & 0b11110), 13);
            this->m_rs2 = (instruction >> 20) & 0b1111;
            this->m_rs1 = (instruction >> 15) & 0b1111;
            this->m_funct3 = (instruction >> 12) & 0b111;
            break;
        //U-type
        case LUI:
        case AUIPC:
            this->m_format = U_TYPE;
            this->m_imm = instruction & 0b11111111111111111111000000000000;
            this->m_rd = (instruction >> 7) & 0b1111;
            break;
        //J-type
        case JAL:
            this->m_format = J_TYPE;
            this->m_imm = SIGN_EXTEND_TO_32(((instruction >> 11) & 0b100000000000000000000) | (instruction & 0b11111111000000000000) | ((instruction >> 9) & 0b100000000000) | ((instruction >> 20) & 0b11111111110), 21);
            this->m_rd = (instruction >> 7) & 0b1111;
            break;
        default:
            this->m_format = INVALID;
            break;
    }
}

void decoded_inst_t::log(uint8_t indent, uint64_t inst_count) const {
    switch (this->m_format) {
        case R_TYPE:
            irvelog(indent, "opcode = 0x%X", this->m_opcode);
            irvelog(indent, "type   = R");
            irvelog(indent, "funct3 = 0x%X", this->m_funct3);
            irvelog(indent, "funct7 = 0x%X", this->m_funct7);
            irvelog(indent, "rd     = x%u", this->m_rd);
            irvelog(indent, "rs1    = x%u", this->m_rs1);
            irvelog(indent, "rs2    = x%u", this->m_rs2);
            break;
        case I_TYPE:
            irvelog(indent, "opcode = 0x%X", this->m_opcode);
            irvelog(indent, "type   = I");
            irvelog(indent, "funct3 = 0x%X", this->m_funct3);
            irvelog(indent, "rd     = x%u", this->m_rd);
            irvelog(indent, "rs1    = x%u", this->m_rs1);
            irvelog(indent, "imm    = 0x%X", this->m_imm);
            break;
        case S_TYPE:
            irvelog(indent, "opcode = 0x%X", this->m_opcode);
            irvelog(indent, "type   = S");
            irvelog(indent, "funct3 = 0x%X", this->m_funct3);
            irvelog(indent, "rs1    = x%u", this->m_rs1);
            irvelog(indent, "rs2    = x%u", this->m_rs2);
            irvelog(indent, "imm    = 0x%X", this->m_imm);
            break;
        case B_TYPE:
            irvelog(indent, "opcode = 0x%X", this->m_opcode);
            irvelog(indent, "type   = B");
            irvelog(indent, "funct3 = 0x%X", this->m_funct3);
            irvelog(indent, "rs1    = x%u", this->m_rs1);
            irvelog(indent, "rs2    = x%u", this->m_rs2);
            irvelog(indent, "imm    = 0x%X", this->m_imm);
            break;
        case U_TYPE:
            irvelog(indent, "opcode = 0x%X", this->m_opcode);
            irvelog(indent, "type   = U");
            irvelog(indent, "rd     = x%u", this->m_rd);
            irvelog(indent, "rs1    = x%u", this->m_rs1);
            irvelog(indent, "imm    = 0x%X", this->m_imm);
            break;
        case J_TYPE:
            irvelog(indent, "opcode = 0x%X", this->m_opcode);
            irvelog(indent, "type   = J");
            irvelog(indent, "rd     = x%u", this->m_rd);
            irvelog(indent, "imm    = 0x%X", this->m_imm);
            break;
        case INVALID:
            irvelog(indent, "Invalid Instruction!");
            break;
    }
}
