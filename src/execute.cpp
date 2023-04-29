/* NAME//TODO
 * By: John Jekel
 *
 * TODO description
 *
*/

/* Constants And Defines */

//TODO

/* Includes */

#include "execute.h"

#include <cassert>
#include <cstdint>

#include "cpu_state.h"
#include "decode.h"

#define INST_COUNT cpu_state.get_inst_count()
#include "logging.h"

/* Types */

//TODO

/* Variables */

//TODO

/* Static Function Declarations */

//TODO

/* Function Implementations */

void execute_op_imm(const decoded_inst_t &decoded_inst, cpu_state_t &cpu_state) {
    irvelog(2, "Executing OP_IMM instruction");

    assert((decoded_inst.get_opcode() == OP_IMM) && "op_imm instruction must have opcode OP_IMM");
    assert((decoded_inst.get_format() == I_TYPE) && "op_imm instruction must be I_TYPE");

    //Get operands
    uint32_t rs1 = cpu_state.get_r(decoded_inst.get_rs1()).u;
    uint32_t imm = decoded_inst.get_imm();

    //Perform the ALU operation
    uint32_t result;
    switch (decoded_inst.get_funct3()) {
        case 0b000://ADDI
            irvelog(3, "Mnemonic: ADDI");
            result = rs1 + imm;
            irvelog(3, "0x%08X + 0x%08X = 0x%08X", rs1, imm, result);
            break;
        case 0b001://SLLI
            irvelog(3, "Mnemonic: SLLI");
            result = rs1 << imm;
            irvelog(3, "0x%08X << 0x%08X = 0x%08X", rs1, imm, result);
            break;
        case 0b010://SLTI
            irvelog(3, "Mnemonic: SLTI");
            result = ((int32_t)rs1 < (int32_t)imm) ? 1 : 0;
            irvelog(3, "(0x%08X < 0x%08X) signed ? 1 : 0 = 0x%08X", rs1, imm, result);
            break;
        case 0b011://SLTIU
            irvelog(3, "Mnemonic: SLTIU");
            result = (rs1 < imm) ? 1 : 0;
            irvelog(3, "(0x%08X < 0x%08X) unsigned ? 1 : 0 = 0x%08X", rs1, imm, result);
            break;
        case 0b100://XORI
            irvelog(3, "Mnemonic: XORI");
            result = rs1 ^ imm;
            irvelog(3, "0x%08X ^ 0x%08X = 0x%08X", rs1, imm, result);
            break;
        case 0b101://SRLI or SRAI
            if (decoded_inst.get_funct7() == 0b0000000) {//SRLI
                irvelog(3, "Mnemonic: SRLI");
                result = rs1 >> imm;
                irvelog(3, "0x%08X >> 0x%08X = 0x%08X", rs1, imm, result);
            } else if (decoded_inst.get_funct7() == 0b0100000) {//SRAI
                irvelog(3, "Mnemonic: SRAI");
                result = (uint32_t)(((int32_t)rs1) >> imm);
                irvelog(3, "(0x%08X signed >> 0x%08X) signed = 0x%08X", rs1, imm, result);
            } else {
                assert(false && "Invalid funct7 for SRLI or SRAI");//TODO handle this
            }
            break;
        case 0b110://ORI
            irvelog(3, "Mnemonic: ORI");
            result = rs1 | imm;
            irvelog(3, "0x%08X | 0x%08X = 0x%08X", rs1, imm, result);
            break;
        case 0b111://ANDI
            irvelog(3, "Mnemonic: ANDI");
            result = rs1 & imm;
            irvelog(3, "0x%08X & 0x%08X = 0x%08X", rs1, imm, result);
            break;
        default:
            assert(false && "We should never get here");
            break;
    }
    irvelog(3, "Overwriting 0x%08X currently in register x%u with 0x%08X", cpu_state.get_r(decoded_inst.get_rd()).u, decoded_inst.get_rd(), result);
    cpu_state.set_r(decoded_inst.get_rd(), result);

    //Increment PC
    cpu_state.set_pc(cpu_state.get_pc() + 4);
    irvelog(3, "Going to next sequential PC: 0x%08X", cpu_state.get_pc()); 
}

/* Static Function Implementations */

//TODO
