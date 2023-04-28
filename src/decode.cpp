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

void decode(decoded_inst_t* decoded_inst, uint32_t instruction, uint64_t inst_count) {
    irvelog(1, "Decoding instruction 0x%08X", instruction);

    decoded_inst->invalid = !instruction || (instruction == 0xFFFFFFFF);
    decoded_inst->opcode = (opcode_t) ((instruction >> 2) & 0b11111);

    switch (decoded_inst->opcode) {
        //R-type
        case OP:
            irvelog(2, "format = R");
            decoded_inst->funct7 = (instruction >> 25) & 0b1111111;
            decoded_inst->rs2 = (instruction >> 20) & 0b1111;
            decoded_inst->rs1 = (instruction >> 15) & 0b1111;
            decoded_inst->funct3 = (instruction >> 12) & 0b111;
            decoded_inst->rd = (instruction >> 7) & 0b1111;
            break;
        //I-type
        case LOAD:
        case OP_IMM:
        case JALR:
        case SYSTEM:
            irvelog(2, "format = I");
            decoded_inst->imm = SIGN_EXTEND_TO_32(instruction >> 20, 12);
            decoded_inst->rs1 = (instruction >> 15) & 0b1111;
            decoded_inst->funct3 = (instruction >> 12) & 0b111;
            decoded_inst->rd = (instruction >> 7) & 0b1111;
            decoded_inst->funct7 = (decoded_inst->funct3 == 0b101) ? instruction >> 25 : 0;//We need funct7 for immediate shifts, otherwise it should be 0 so that execute isn't confused
            break;
        //S-type
        case STORE:
            irvelog(2, "format = S");
            decoded_inst->imm = SIGN_EXTEND_TO_32(((instruction >> 20) & 0b111111100000) | ((instruction >> 7) & 0b11111), 12);
            decoded_inst->rs2 = (instruction >> 20) & 0b1111;
            decoded_inst->rs1 = (instruction >> 15) & 0b1111;
            decoded_inst->funct3 = (instruction >> 12) & 0b111;
            break;
        //B-type
        case BRANCH:
            irvelog(2, "format = B");
            decoded_inst->imm = SIGN_EXTEND_TO_32(((instruction >> 19) & 0b1000000000000) | ((instruction << 4) & 0b100000000000) | ((instruction >> 20) & 0b11111100000) | ((instruction >> 7) & 0b11110), 13);
            decoded_inst->rs2 = (instruction >> 20) & 0b1111;
            decoded_inst->rs1 = (instruction >> 15) & 0b1111;
            decoded_inst->funct3 = (instruction >> 12) & 0b111;
            break;
        //U-type
        case LUI:
        case AUIPC:
            irvelog(2, "format = U");
            decoded_inst->imm = instruction & 0b11111111111111111111000000000000;
            decoded_inst->rd = (instruction >> 7) & 0b1111;
            break;
        //J-type
        case JAL:
            irvelog(2, "format = J");
            decoded_inst->imm = SIGN_EXTEND_TO_32(((instruction >> 11) & 0b100000000000000000000) | (instruction & 0b11111111000000000000) | ((instruction >> 9) & 0b100000000000) | ((instruction >> 20) & 0b11111111110), 21);
            decoded_inst->rd = (instruction >> 7) & 0b1111;
            break;
        default:
            irvelog(2, "format = Unknown/Custom");
    }

    irvelog(2, "funct3 = 0x%X", decoded_inst->funct3);
    irvelog(2, "funct7 = 0x%X", decoded_inst->funct7);
    irvelog(2, "opcode = 0x%X", decoded_inst->opcode);
    irvelog(2, "rd = x%u", decoded_inst->rd);
    irvelog(2, "rs1 = x%u", decoded_inst->rs1);
    irvelog(2, "rs2 = x%u", decoded_inst->rs2);
    irvelog(2, "imm = 0x%X", decoded_inst->imm);
    irvelog(2, "invalid = %s", decoded_inst->invalid ? "Yes" : "No");
}
