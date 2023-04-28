/* decode.h
 * By: John Jekel
 *
 * Code to decode RISC-V instructions
 *
 * Based on code from rv32esim
 *
*/

#ifndef DECODE_H
#define DECODE_H

/* Includes */

#include <cstdint>

/* Types */

///opcode_t is an enum of RISC-V opcodes
typedef enum {
    LOAD = 0b00000, LOAD_FP = 0b00001, CUSTOM_0 = 0b00010, MISC_MEM = 0b00011, OP_IMM = 0b00100, AUIPC = 0b00101, OP_IMM_32 = 0b00110, B48_0 = 0b00111,
    STORE = 0b01000, STORE_FP = 0b01001, CUSTOM_1 = 0b01010, AMO = 0b01011, OP = 0b01100, LUI = 0b01101, OP_32 = 0b01110, B64 = 0b01111,
    MADD = 0b10000, MSUB = 0b10001, NMSUB = 0b10011, OP_FP = 0b10100, RESERVED_0 = 0b10101, CUSTOM_2 = 0b10110, B48_1 = 0b10111,
    BRANCH = 0b11000, JALR = 0b11001, RESERVED_1 = 0b11010, JAL = 0b11011, SYSTEM = 0b11100, RESERVED_3 = 0b11101, CUSTOM_3 = 0b11110, BGE80 = 0b11111,
} opcode_t;

///decode_inst_t is a type holding the results from decoding a RISC-V instruction
//NOTE: We are NOT supporting compressed instructions
class decoded_inst_t {
public:
    void log(uint8_t indent) const;

    uint8_t funct3;
    uint8_t funct7;
    opcode_t opcode;//Bits [6:2]
    uint8_t rd;
    uint8_t rs1;
    uint8_t rs2;
    uint32_t imm;
    bool invalid;
};

/* Function/Class Declarations */

void decode(decoded_inst_t* decoded_inst, uint32_t instruction, uint64_t inst_count);
///decode decodes a RISC-V instruction into a decoded_inst
//void decode(const rv32esim_state_t* state, decoded_inst_t* decoded_inst, uint32_t instruction);

//TODO

#endif//DECODE_H
