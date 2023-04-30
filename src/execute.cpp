/* execute.cpp
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Utility functions for executing instructions
 *
*/

/* Includes */

#include "execute.h"

#include <cassert>
#include <cstdint>

#include "cpu_state.h"
#include "decode.h"

#define INST_COUNT cpu_state.get_inst_count()
#include "logging.h"

/* Static Function Declarations */

//TODO

/* Function Implementations */

void execute_load(const decoded_inst_t &decoded_inst, cpu_state_t &cpu_state, memory_t &memory) {
    assert(false && "TODO implement execute_load()");
}

void execute_misc_mem(const decoded_inst_t &decoded_inst, cpu_state_t &cpu_state, memory_t &memory) {
    assert(false && "TODO implement execute_misc_mem()");
}

void execute_op_imm(const decoded_inst_t &decoded_inst, cpu_state_t &cpu_state) {
    irvelog(2, "Executing OP_IMM instruction");

    assert((decoded_inst.get_opcode() == OP_IMM) && "op_imm instruction must have opcode OP_IMM");
    assert((decoded_inst.get_format() == I_TYPE) && "op_imm instruction must be I_TYPE");

    //Get operands
    reg_t rs1 = cpu_state.get_r(decoded_inst.get_rs1());
    reg_t imm;
    imm.u = decoded_inst.get_imm();

    //Perform the ALU operation
    reg_t result;
    switch (decoded_inst.get_funct3()) {
        case 0b000://ADDI
            irvelog(3, "Mnemonic: ADDI");
            result.s = rs1.s + imm.s;
            irvelog(3, "0x%08X + 0x%08X = 0x%08X", rs1.u, imm.u, result.u);
            break;
        case 0b001://SLLI
            irvelog(3, "Mnemonic: SLLI");
            result.u = rs1.u << (imm.u & 0b11111);
            irvelog(3, "0x%08X << 0x%08X = 0x%08X", rs1.u, imm.u, result.u);
            break;
        case 0b010://SLTI
            irvelog(3, "Mnemonic: SLTI");
            result.u = (rs1.s < imm.s) ? 1 : 0;
            irvelog(3, "(0x%08X signed < 0x%08X signed) ? 1 : 0 = 0x%08X", rs1.u, imm.u, result.u);
            break;
        case 0b011://SLTIU
            irvelog(3, "Mnemonic: SLTIU");
            result.u = (rs1.u < imm.u) ? 1 : 0;
            irvelog(3, "(0x%08X unsigned < 0x%08X unsigned) ? 1 : 0 = 0x%08X", rs1.u, imm.u, result.u);
            break;
        case 0b100://XORI
            irvelog(3, "Mnemonic: XORI");
            result.u = rs1.u ^ imm.u;
            irvelog(3, "0x%08X ^ 0x%08X = 0x%08X", rs1.u, imm.u, result.u);
            break;
        case 0b101://SRLI or SRAI
            //FIXME we can't access the funct7 field of an I_TYPE instruction; need to fix that in either decode.cpp or here
            if (decoded_inst.get_funct7() == 0b0000000) {//SRLI
                irvelog(3, "Mnemonic: SRLI");
                result.u = rs1.u >> (imm.u & 0b11111);
                irvelog(3, "0x%08X >> 0x%08X logical = 0x%08X", rs1.u, imm.u, result.u);
            } else if (decoded_inst.get_funct7() == 0b0100000) {//SRAI
                irvelog(3, "Mnemonic: SRAI");
                result.s = rs1.s >> (imm.s & 0b11111);
                irvelog(3, "0x%08X >> 0x%08X arithmetic = 0x%08X", rs1.u, imm.u, result.u);
            } else {
                assert(false && "Invalid funct7 for SRLI or SRAI");//TODO handle this
            }
            break;
        case 0b110://ORI
            irvelog(3, "Mnemonic: ORI");
            result.u = rs1.u | imm.u;
            irvelog(3, "0x%08X | 0x%08X = 0x%08X", rs1.u, imm.u, result.u);
            break;
        case 0b111://ANDI
            irvelog(3, "Mnemonic: ANDI");
            result.u = rs1.u & imm.u;
            irvelog(3, "0x%08X & 0x%08X = 0x%08X", rs1.u, imm.u, result.u);
            break;
        default:
            assert(false && "We should never get here");
            break;
    }
    irvelog(3, "Overwriting 0x%08X currently in register x%u with 0x%08X", cpu_state.get_r(decoded_inst.get_rd()).u, decoded_inst.get_rd(), result);
    cpu_state.set_r(decoded_inst.get_rd(), result.u);

    //Increment PC
    cpu_state.set_pc(cpu_state.get_pc() + 4);
    irvelog(3, "Going to next sequential PC: 0x%08X", cpu_state.get_pc()); 
}

void execute_auipc(const decoded_inst_t &decoded_inst, cpu_state_t &cpu_state) {
    assert(false && "TODO implement execute_auipc()");
}

void execute_store(const decoded_inst_t &decoded_inst, cpu_state_t &cpu_state, memory_t &memory) {
    irvelog(2, "Executing store instruction");

    assert((decoded_inst.get_opcode() == STORE) && "store instruction must have opcode STORE");
    assert((decoded_inst.get_format() == S_TYPE) && "store instruction must be S_TYPE");

    // Get operands
    reg_t r1 = cpu_state.get_r(decoded_inst.get_rs1());
    reg_t r2 = cpu_state.get_r(decoded_inst.get_rs2());
    reg_t imm;
    imm.u = decoded_inst.get_imm();
    uint8_t func3 = decoded_inst.get_funct3();

    switch(func3) {
        case 0b000://SB
            irvelog(3, "Mnemonic: SB");
            break;
        case 0b001://SH
            irvelog(3, "Mnemonic: SH");
            break;
        case 0b010://SW
            irvelog(3, "Mnemonic: SW");
            break;
        default:
            assert(false && "We should never get here");
            break;
    }
    // TODO what else should this log?
    try {
        // Note this will throw an excepteion if the memory address isn't valid (TODO)
        memory.w(r1.u + imm.u, func3, r2.s);
    }
    catch(...) {
        // TODO what happens when we access invalid memory?
    }

    //Increment PC
    cpu_state.set_pc(cpu_state.get_pc() + 4);
    irvelog(3, "Going to next sequential PC: 0x%08X", cpu_state.get_pc()); 
}

void execute_amo(const decoded_inst_t &decoded_inst, cpu_state_t &cpu_state, memory_t &memory) {
    assert(false && "TODO implement execute_amo()");
}

void execute_op(const decoded_inst_t &decoded_inst, cpu_state_t &cpu_state) {
    irvelog(2, "Executing OP instruction"); 

    assert((decoded_inst.get_opcode() == OP) && "op instruction must have opcode OP");
    assert((decoded_inst.get_format() == R_TYPE) && "op instruction must be R_TYPE");

    //Get operands
    reg_t rs1 = cpu_state.get_r(decoded_inst.get_rs1());
    reg_t rs2 = cpu_state.get_r(decoded_inst.get_rs2());

    //Perform the ALU operation
    reg_t result;
    switch (decoded_inst.get_funct3()) {
        case 0b000://ADD or SUB
            if (decoded_inst.get_funct7() == 0b0000000) {//ADD
                irvelog(3, "Mnemonic: ADD");
                result.u = rs1.u + rs2.u;
                irvelog(3, "0x%08X + 0x%08X = 0x%08X", rs1.u, rs2.u, result);
            } else if (decoded_inst.get_funct7() == 0b0100000) {//SUB
                irvelog(3, "Mnemonic: SUB");
                result.u = rs1.u - rs2.u;
                irvelog(3, "0x%08X - 0x%08X = 0x%08X", rs1.u, rs2.u, result);
            } else {
                assert(false && "Invalid funct7 for ADD or SUB");//TODO handle this
            }
            break;
        case 0b001://SLL
            irvelog(3, "Mnemonic: SLL");
            result.u = rs1.u << (rs2.u & 0b11111);
            irvelog(3, "0x%08X << 0x%08X logical = 0x%08X", rs1.u, rs2.u, result);
            break;
        case 0b010://SLT
            irvelog(3, "Mnemonic: SLT");
            result.s = rs1.s < rs2.s;
            irvelog(3, "(0x%08X signed < 0x%08X signed) = 0x%08X", rs1.u, rs2.u, result);
            break;
        case 0b011://SLTU
            irvelog(3, "Mnemonic: SLTU");
            result.u = rs1.u < rs2.u;
            irvelog(3, "(0x%08X unsigned < 0x%08X unsigned) = 0x%08X", rs1.u, rs2, result);
            break;
        case 0b100://XOR
            irvelog(3, "Mnemonic: XOR");
            result.u = rs1.u ^ rs2.u;
            irvelog(3, "0x%08X ^ 0x%08X = 0x%08X", rs1.u, rs2.u, result);
            break;
        case 0b101://SRL or SRA
            if (decoded_inst.get_funct7() == 0b0000000) {//SRL
                irvelog(3, "Mnemonic: SRL");
                result.u = rs1.u >> (rs2.u & 0b11111);
                irvelog(3, "0x%08X >> 0x%08X logical = 0x%08X", rs1.u, rs2.u, result.u);
            } else if (decoded_inst.get_funct7() == 0b0100000) {//SRA
                irvelog(3, "Mnemonic: SRA");
                result.s = rs1.s >> (rs2.s & 0b11111);
                irvelog(3, "0x%08X >> 0x%08X arithmetic = 0x%08X", rs1.u, rs2.u, result.u);
            } else {
                assert(false && "Invalid funct7 for SRL or SRA");//TODO handle this
            }
            break;
        case 0b110://OR
            irvelog(3, "Mnemonic: OR");
            result.u = rs1.u | rs2.u;
            irvelog(3, "0x%08X | 0x%08X = 0x%08X", rs1.u, rs2.u, result);
            break;
        case 0b111://AND
            irvelog(3, "Mnemonic: AND");
            result.u = rs1.u & rs2.u;
            irvelog(3, "0x%08X & 0x%08X = 0x%08X", rs1.u, rs2.u, result);
            break;
        default:
            assert(false && "We should never get here");
            break;
    }
    irvelog(3, "Overwriting 0x%08X currently in register x%u with 0x%08X", cpu_state.get_r(decoded_inst.get_rd()).u, decoded_inst.get_rd(), result);
    cpu_state.set_r(decoded_inst.get_rd(), result.u);

    //Increment PC
    cpu_state.set_pc(cpu_state.get_pc() + 4);
    irvelog(3, "Going to next sequential PC: 0x%08X", cpu_state.get_pc()); 
}

void execute_lui(const decoded_inst_t &decoded_inst, cpu_state_t &cpu_state) {
    assert(false && "TODO implement execute_lui()");
}

void execute_branch(const decoded_inst_t &decoded_inst, cpu_state_t &cpu_state) {
    assert(false && "TODO implement execute_branch()");
}

void execute_jalr(const decoded_inst_t &decoded_inst, cpu_state_t &cpu_state) {
    assert(false && "TODO implement execute_jalr()");
}

void execute_jal(const decoded_inst_t &decoded_inst, cpu_state_t &cpu_state) {
    assert(false && "TODO implement execute_jal()");
}

void execute_system(const decoded_inst_t &decoded_inst, cpu_state_t &cpu_state, memory_t &memory) {
    assert(false && "TODO implement execute_system()");
}

/* Static Function Implementations */

//TODO
