/* execute.cpp
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Utility functions for executing instructions
 *
 * Some inspiration from rv32esim
 *
*/

//TODO transition to using word_t and reg_t

/* Includes */

#include "execute.h"

#include <cassert>
#include <cstdint>

#include "cpu_state.h"
#include "decode.h"
#include "rvexception.h"

#define INST_COUNT cpu_state.get_inst_count()
#include "logging.h"

/* Static Function Declarations */

static void goto_next_sequential_pc(cpu_state_t& cpu_state);

/* Function Implementations */

void execute::load(const decoded_inst_t& decoded_inst, cpu_state_t& cpu_state, memory_t& memory) {
    irvelog(2, "Executing LOAD instruction");

    assert((decoded_inst.get_opcode() == LOAD) && "load instruction must have opcode LOAD");
    assert((decoded_inst.get_format() == I_TYPE) && "load instruction must be I_TYPE");

    // Get operands
    reg_t r1 = cpu_state.get_r(decoded_inst.get_rs1());
    reg_t imm = decoded_inst.get_imm();
    uint8_t funct3 = decoded_inst.get_funct3();

    switch (funct3) {
        case 0b000://LB
            irvelog(3, "Mnemonic: LB");
            break;
        case 0b001://LH
            irvelog(3, "Mnemonic: LH");
            break;
        case 0b010://LW
            irvelog(3, "Mnemonic: LW");
            break;
        case 0b100://LBU
            irvelog(3, "Mnemonic: LBU");
            break;
        case 0b101://LHU
            irvelog(3, "Mnemonic: LHU");
            break;
        default:
            throw rvexception_t(ILLEGAL_INSTRUCTION_EXCEPTION);
            break;
    }
    try {
        word_t loaded = memory.r(r1 + imm, funct3);
        irvelog(3, "Loaded 0x%08X from 0x%08X", loaded.s, (r1 + imm).u);
        cpu_state.set_r(decoded_inst.get_rd(), loaded);
    }
    catch(...) {
        // TODO what happens when we access invalid memory?
        //Actually we probably shouldn't catch here, but rather pass this up to the emulator
        //TODO priority of this exception vs. the illegal instruction exception?
    }

    //Increment PC
    goto_next_sequential_pc(cpu_state);
}

void execute::custom_0(const decoded_inst_t& decoded_inst, cpu_state_t& cpu_state, memory_t& memory) {
    irvelog(2, "Executing custom-0 instruction");

    assert((decoded_inst.get_opcode() == CUSTOM_0) && "custom-0 instruction must have opcode CUSTOM_0");
    assert((decoded_inst.get_format() == R_TYPE) && "custom-0 instruction must be R_TYPE");

    //All other fields being zero means emulator exit request
    if (!decoded_inst.get_rd() && !decoded_inst.get_funct3() && !decoded_inst.get_rs1() && !decoded_inst.get_rs2() && !decoded_inst.get_funct7()) {
        irvelog(3, "Mnemonic: IRVE.EXIT");
        if (cpu_state.get_privilege_mode() == privilege_mode_t::MACHINE_MODE) {
            irvelog(3, "In machine mode, so the IRVE.EXIT instruction is valid");
            throw irve_exit_request_t();
        } else {
            irvelog(3, "The IRVE.EXIT instruction is only valid in machine mode; treating as an illegal instruction");
            throw rvexception_t(ILLEGAL_INSTRUCTION_EXCEPTION);
        }
    } else {//Otherwise we don't implement any others for now
        throw rvexception_t(ILLEGAL_INSTRUCTION_EXCEPTION);
    }
}

void execute::misc_mem(const decoded_inst_t& decoded_inst, cpu_state_t& cpu_state) {
    irvelog(2, "Executing MISC-MEM instruction");

    if (decoded_inst.get_funct3() == 0b000) {//FENCE
        irvelog(3, "Mnemonic: FENCE");
    } else if (decoded_inst.get_funct3() == 0b001) {//FENCE.I
        irvelog(3, "Mnemonic: FENCE.I");
    } else {
        throw rvexception_t(ILLEGAL_INSTRUCTION_EXCEPTION);
    }

    irvelog(3, "Nothing to do since the emulated system dosn't have a cache or multiple harts");

    //Increment PC
    goto_next_sequential_pc(cpu_state);
}

void execute::op_imm(const decoded_inst_t& decoded_inst, cpu_state_t& cpu_state) {
    irvelog(2, "Executing OP-IMM instruction");

    assert((decoded_inst.get_opcode() == OP_IMM) && "op_imm instruction must have opcode OP_IMM");
    assert((decoded_inst.get_format() == I_TYPE) && "op_imm instruction must be I_TYPE");

    //Get operands
    reg_t rs1 = cpu_state.get_r(decoded_inst.get_rs1());
    word_t imm = decoded_inst.get_imm();

    //Perform the ALU operation
    word_t result;
    switch (decoded_inst.get_funct3()) {
        case 0b000://ADDI
            irvelog(3, "Mnemonic: ADDI");
            result = rs1 + imm;
            irvelog(3, "0x%08X + 0x%08X = 0x%08X", rs1.u, imm.u, result.u);
            break;
        case 0b001://SLLI
            irvelog(3, "Mnemonic: SLLI");
            result = rs1 << imm.bits(4, 0);
            irvelog(3, "0x%08X << 0x%08X = 0x%08X", rs1.u, imm.u, result.u);
            break;
        case 0b010://SLTI
            irvelog(3, "Mnemonic: SLTI");
            result = (rs1.s < imm.s) ? 1 : 0;
            irvelog(3, "(0x%08X signed < 0x%08X signed) ? 1 : 0 = 0x%08X", rs1.u, imm.u, result.u);
            break;
        case 0b011://SLTIU
            irvelog(3, "Mnemonic: SLTIU");
            result = (rs1.u < imm.u) ? 1 : 0;
            irvelog(3, "(0x%08X unsigned < 0x%08X unsigned) ? 1 : 0 = 0x%08X", rs1.u, imm.u, result.u);
            break;
        case 0b100://XORI
            irvelog(3, "Mnemonic: XORI");
            result = rs1 ^ imm;
            irvelog(3, "0x%08X ^ 0x%08X = 0x%08X", rs1.u, imm.u, result.u);
            break;
        case 0b101://SRLI or SRAI
            if (decoded_inst.get_funct7() == 0b0000000) {//SRLI
                irvelog(3, "Mnemonic: SRLI");
                result = rs1.srl(imm.bits(4, 0));
                irvelog(3, "0x%08X >> 0x%08X logical = 0x%08X", rs1.u, imm.u, result.u);
            } else if (decoded_inst.get_funct7() == 0b0100000) {//SRAI
                irvelog(3, "Mnemonic: SRAI");
                result = rs1.sra(imm.bits(4, 0));
                irvelog(3, "0x%08X >> 0x%08X arithmetic = 0x%08X", rs1.u, imm.u, result.u);
            } else {
                throw rvexception_t(ILLEGAL_INSTRUCTION_EXCEPTION);
            }
            break;
        case 0b110://ORI
            irvelog(3, "Mnemonic: ORI");
            result = rs1 | imm;
            irvelog(3, "0x%08X | 0x%08X = 0x%08X", rs1.u, imm.u, result.u);
            break;
        case 0b111://ANDI
            irvelog(3, "Mnemonic: ANDI");
            result = rs1 & imm;
            irvelog(3, "0x%08X & 0x%08X = 0x%08X", rs1.u, imm.u, result.u);
            break;
        default:
            assert(false && "We should never get here");
            break;
    }
    irvelog(3, "Overwriting 0x%08X currently in register x%u with 0x%08X", cpu_state.get_r(decoded_inst.get_rd()).u, decoded_inst.get_rd(), result);
    cpu_state.set_r(decoded_inst.get_rd(), result.u);

    //Increment PC
    goto_next_sequential_pc(cpu_state);
}

void execute::auipc(const decoded_inst_t& decoded_inst, cpu_state_t& cpu_state) {
    irvelog(2, "Executing AUIPC instruction");

    assert((decoded_inst.get_opcode() == AUIPC) && "auipc instruction must have opcode AUIPC");
    assert((decoded_inst.get_format() == U_TYPE) && "auipc instruction must be U_TYPE");

    word_t result = decoded_inst.get_imm() + cpu_state.get_pc();

    irvelog(3, "Overwriting 0x%08X currently in register x%u with 0x%08X", cpu_state.get_r(decoded_inst.get_rd()).u, decoded_inst.get_rd(), result.u);
    cpu_state.set_r(decoded_inst.get_rd(), result);

    goto_next_sequential_pc(cpu_state);
}

void execute::store(const decoded_inst_t& decoded_inst, cpu_state_t& cpu_state, memory_t& memory) {
    irvelog(2, "Executing STORE instruction");

    assert((decoded_inst.get_opcode() == STORE) && "store instruction must have opcode STORE");
    assert((decoded_inst.get_format() == S_TYPE) && "store instruction must be S_TYPE");

    // Get operands
    reg_t r1 = cpu_state.get_r(decoded_inst.get_rs1());
    reg_t r2 = cpu_state.get_r(decoded_inst.get_rs2());
    word_t imm = decoded_inst.get_imm();
    uint8_t funct3 = decoded_inst.get_funct3();

    switch(funct3) {
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
            throw rvexception_t(ILLEGAL_INSTRUCTION_EXCEPTION);
            break;
    }
    
    try {
        // Note this will throw an excepteion if the memory address isn't valid (TODO)
        memory.w(r1.u + imm.u, funct3, r2.s);
        irvelog(3, "Stored 0x%08X in 0x%08X", r2.u, r1.u + imm.u);
    }
    catch(...) {
        assert(false && "TODO");// TODO what happens when we access invalid memory?
        //Actually we probably shouldn't catch here, but rather pass this up to the emulator
        //TODO priority of this exception vs. the illegal instruction exception?
    }

    //Increment PC
    goto_next_sequential_pc(cpu_state);
}

void execute::amo(const decoded_inst_t& decoded_inst, cpu_state_t& cpu_state, memory_t& memory) {
    irvelog(2, "Executing AMO instruction");

    //Sanity checks
    assert((decoded_inst.get_opcode() == AMO) && "amo instruction must have opcode AMO");
    assert((decoded_inst.get_format() == R_TYPE) && "amo instruction must be R_TYPE");
    if (decoded_inst.get_funct3() != 0b010) {
        throw rvexception_t(ILLEGAL_INSTRUCTION_EXCEPTION);
    }
    //NOTE: All possible aq and rl values are valid, so we don't need to check them

    //Get operands
    reg_t rs1 = cpu_state.get_r(decoded_inst.get_rs1());
    reg_t rs2 = cpu_state.get_r(decoded_inst.get_rs2());

    //Check that the address is word-aligned
    if ((rs1.u % 4) != 0) {
        //NOTE: This exception has priority over access faults but not over the illegal instruction exception
        throw rvexception_t(STORE_OR_AMO_ADDRESS_MISALIGNED_EXCEPTION);
    }

    word_t loaded_word;
    switch (decoded_inst.get_funct5()) {
        case 0b00010://LR.W
            irvelog(3, "Mnemonic: LR.W");
            //TODO what if the CPU is interrupted between the load and the store?
            assert(false && "TODO");
            break;
        case 0b00011://SC.W
            irvelog(3, "Mnemonic: SC.W");
            //TODO what if the CPU is interrupted between the load and the store?
            assert(false && "TODO");
            break;
        case 0b00001://AMOSWAP.W
            irvelog(3, "Mnemonic: AMOSWAP.W");

            //Read the word at the address in rs1
            try {
                loaded_word = memory.r(rs1, 0b010);
            } catch (const rvexception_t& e) {//If we get an exception, we need to rethrow a different one to indicate this is due to an AMO instruction
                switch (e.cause()) {//TODO ensure this is correct
                    case cause_t::LOAD_ADDRESS_MISALIGNED_EXCEPTION:
                        assert(false && "Got a misaligned address exception when reading from memory, but we already checked that the address was aligned!");
                        break;
                    case cause_t::LOAD_ACCESS_FAULT_EXCEPTION:
                        throw rvexception_t(STORE_OR_AMO_ACCESS_FAULT_EXCEPTION);
                        break;
                    case cause_t::LOAD_PAGE_FAULT_EXCEPTION:
                        throw rvexception_t(STORE_OR_AMO_PAGE_FAULT_EXCEPTION);
                        break;
                    default:
                        assert(false && "Unexpected exception when reading from memory");
                        break;
                }
            }

            //Save it into rd
            cpu_state.set_r(decoded_inst.get_rd(), loaded_word);

            //Swap, so now write rs2 into the address in rs1
            memory.w(rs1, 0b010, rs2.s);

            assert(false && "TODO");
            break;
        case 0b00000://AMOADD.W
            irvelog(3, "Mnemonic: AMOADD.W");
            assert(false && "TODO");
            break;
        case 0b00100://AMOXOR.W
            irvelog(3, "Mnemonic: AMOXOR.W");
            assert(false && "TODO");
            break;
        case 0b01100://AMOAND.W
            irvelog(3, "Mnemonic: AMOAND.W");
            assert(false && "TODO");
            break;
        case 0b01000://AMOOR.W
            irvelog(3, "Mnemonic: AMOOR.W");
            assert(false && "TODO");
            break;
        case 0b10000://AMOMIN.W
            irvelog(3, "Mnemonic: AMOMIN.W");
            assert(false && "TODO");
            break;
        case 0b10100://AMOMAX.W
            irvelog(3, "Mnemonic: AMOMAX.W");
            assert(false && "TODO");
            break;
        case 0b11000://AMOMINU.W
            irvelog(3, "Mnemonic: AMOMINU.W");
            assert(false && "TODO");
            break;
        case 0b11100://AMOMAXU.W
            irvelog(3, "Mnemonic: AMOMAXU.W");
            assert(false && "TODO");
            break;
        default:
            throw rvexception_t(ILLEGAL_INSTRUCTION_EXCEPTION);
            break;
    }

    goto_next_sequential_pc(cpu_state);
}

void execute::op(const decoded_inst_t& decoded_inst, cpu_state_t& cpu_state) {
    irvelog(2, "Executing OP instruction"); 

    assert((decoded_inst.get_opcode() == OP) && "op instruction must have opcode OP");
    assert((decoded_inst.get_format() == R_TYPE) && "op instruction must be R_TYPE");

    //Get operands
    reg_t rs1 = cpu_state.get_r(decoded_inst.get_rs1());
    reg_t rs2 = cpu_state.get_r(decoded_inst.get_rs2());

    //Perform the ALU operation
    word_t result;
    if (decoded_inst.get_funct7() == 0b0000001) {//M extension instructions
        switch (decoded_inst.get_funct3()) {
            case 0b000://MUL
                irvelog(3, "Mnemonic: MUL");

                result = rs1 * rs2;
                irvelog(3, "0x%08X * 0x%08X = 0x%08X", rs1.u, rs2.u, result);

                break;
            case 0b001://MULH
                irvelog(3, "Mnemonic: MULH");

                //TODO ensure casting to int64_t actually performs sign extension
                result = (uint32_t)((((int64_t)rs1.s) * ((int64_t)rs2.s)) >> 32);

                irvelog(3, "0x%08X signed * 0x%08X signed upper half = 0x%08X", rs1.s, rs2.s, result);
                break;
            case 0b010://MULHSU
                irvelog(3, "Mnemonic: MULHSU");

                //TODO ensure casting to int64_t actually performs sign extension ONLY WHEN CASTING rs1.s since it is signed
                result = (uint32_t)((((int64_t)rs1.s) * ((int64_t)rs2.u)) >> 32);

                irvelog(3, "0x%08X signed * 0x%08X unsigned upper half = 0x%08X", rs1.s, rs2.u, result);
                break;
            case 0b011://MULHU
                irvelog(3, "Mnemonic: MULHU");

                result = (uint32_t)((((uint64_t)rs1.u) * ((uint64_t)rs2.u)) >> 32);

                irvelog(3, "0x%08X unsigned * 0x%08X unsigned upper half = 0x%08X", rs1.u, rs2.u, result);
                break;
            case 0b100://DIV
                irvelog(3, "Mnemonic: DIV");

                if (!rs2) {//Division by zero
                    result = 0xFFFFFFFF;
                } else if ((rs1 == 0x80000000) && (rs2 == -1)) {//Overflow (division of the most negative number by -1)
                    result = 0x80000000;
                } else {
                    result = rs1.s / rs2.s;
                }

                irvelog(3, "0x%08X signed / 0x%08X signed = 0x%08X", rs1.s, rs2.s, result);
                break;
            case 0b101://DIVU
                irvelog(3, "Mnemonic: DIVU");

                if (!rs2) {//Division by zero
                    result = 0xFFFFFFFF;
                } else {
                    result = rs1.u / rs2.u;
                }

                irvelog(3, "0x%08X unsigned / 0x%08X unsigned = 0x%08X", rs1.u, rs2.u, result);
                break;
            case 0b110://REM
                irvelog(3, "Mnemonic: REM");

                if (!rs2) {//Division by zero
                    result = rs1;
                } else if ((rs1 == 0x80000000) && (rs2 == -1)) {//Overflow (division of the most negative number by -1)
                    result = 0;
                } else {
                    result = rs1.s % rs2.s;
                }

                irvelog(3, "0x%08X signed %% 0x%08X signed = 0x%08X", rs1.s, rs2.s, result);
                break;
            case 0b111://REMU
                irvelog(3, "Mnemonic: REMU");

                if (!rs2) {//Division by zero
                    result = rs1;
                } else {
                    result = rs1.u % rs2.u;
                }

                irvelog(3, "0x%08X unsigned %% 0x%08X unsigned = 0x%08X", rs1.u, rs2.u, result);
                break;
            default:
                assert(false && "We should never get here");
                break;
        }
    } else {//Others (base spec)
        switch (decoded_inst.get_funct3()) {
            case 0b000://ADD or SUB
                if (decoded_inst.get_funct7() == 0b0000000) {//ADD
                    irvelog(3, "Mnemonic: ADD");
                    result = rs1 + rs2;
                    irvelog(3, "0x%08X + 0x%08X = 0x%08X", rs1.u, rs2.u, result);
                } else if (decoded_inst.get_funct7() == 0b0100000) {//SUB
                    irvelog(3, "Mnemonic: SUB");
                    result = rs1 - rs2;
                    irvelog(3, "0x%08X - 0x%08X = 0x%08X", rs1.u, rs2.u, result);
                } else {
                    throw rvexception_t(ILLEGAL_INSTRUCTION_EXCEPTION);
                }
                break;
            case 0b001://SLL
                irvelog(3, "Mnemonic: SLL");

                if (decoded_inst.get_funct7() != 0b0000000) {
                    throw rvexception_t(ILLEGAL_INSTRUCTION_EXCEPTION);
                }

                result = rs1 << rs2.bits(4, 0);

                irvelog(3, "0x%08X << 0x%08X logical = 0x%08X", rs1.u, rs2.u, result);
                break;
            case 0b010://SLT
                irvelog(3, "Mnemonic: SLT");

                if (decoded_inst.get_funct7() != 0b0000000) {
                    throw rvexception_t(ILLEGAL_INSTRUCTION_EXCEPTION);
                }

                result = (rs1.s < rs2.s) ? 1 : 0;

                irvelog(3, "(0x%08X signed < 0x%08X signed) = 0x%08X", rs1.u, rs2.u, result);
                break;
            case 0b011://SLTU
                irvelog(3, "Mnemonic: SLTU");

                if (decoded_inst.get_funct7() != 0b0000000) {
                    throw rvexception_t(ILLEGAL_INSTRUCTION_EXCEPTION);
                }

                result = (rs1.u < rs2.u) ? 1 : 0;

                irvelog(3, "(0x%08X unsigned < 0x%08X unsigned) = 0x%08X", rs1.u, rs2, result);
                break;
            case 0b100://XOR
                irvelog(3, "Mnemonic: XOR");

                if (decoded_inst.get_funct7() != 0b0000000) {
                    throw rvexception_t(ILLEGAL_INSTRUCTION_EXCEPTION);
                }

                result = rs1 ^ rs2;

                irvelog(3, "0x%08X ^ 0x%08X = 0x%08X", rs1.u, rs2.u, result);
                break;
            case 0b101://SRL or SRA
                if (decoded_inst.get_funct7() == 0b0000000) {//SRL
                    irvelog(3, "Mnemonic: SRL");
                    result = rs1.srl(rs2.bits(4, 0));
                    irvelog(3, "0x%08X >> 0x%08X logical = 0x%08X", rs1.u, rs2.u, result.u);
                } else if (decoded_inst.get_funct7() == 0b0100000) {//SRA
                    irvelog(3, "Mnemonic: SRA");
                    result = rs1.sra(rs2.bits(4, 0));
                    irvelog(3, "0x%08X >> 0x%08X arithmetic = 0x%08X", rs1.u, rs2.u, result.u);
                } else {
                    throw rvexception_t(ILLEGAL_INSTRUCTION_EXCEPTION);
                }
                break;
            case 0b110://OR
                irvelog(3, "Mnemonic: OR");

                if (decoded_inst.get_funct7() != 0b0000000) {
                    throw rvexception_t(ILLEGAL_INSTRUCTION_EXCEPTION);
                }

                result = rs1 | rs2;

                irvelog(3, "0x%08X | 0x%08X = 0x%08X", rs1.u, rs2.u, result);
                break;
            case 0b111://AND
                irvelog(3, "Mnemonic: AND");

                if (decoded_inst.get_funct7() != 0b0000000) {
                    throw rvexception_t(ILLEGAL_INSTRUCTION_EXCEPTION);
                }

                result = rs1 & rs2;

                irvelog(3, "0x%08X & 0x%08X = 0x%08X", rs1.u, rs2.u, result);
                break;
            default:
                assert(false && "We should never get here");
                break;
        }
    }
    irvelog(3, "Overwriting 0x%08X currently in register x%u with 0x%08X", cpu_state.get_r(decoded_inst.get_rd()).u, decoded_inst.get_rd(), result);
    cpu_state.set_r(decoded_inst.get_rd(), result.u);

    goto_next_sequential_pc(cpu_state);
}

void execute::lui(const decoded_inst_t& decoded_inst, cpu_state_t& cpu_state) {
    irvelog(2, "Executing LUI instruction");

    assert((decoded_inst.get_opcode() == LUI) && "lui instruction must have opcode LUI");
    assert((decoded_inst.get_format() == U_TYPE) && "lui instruction must be U_TYPE");

    irvelog(3, "Overwriting 0x%08X currently in register x%u with 0x%08X", cpu_state.get_r(decoded_inst.get_rd()).u, decoded_inst.get_rd(), decoded_inst.get_imm());
    cpu_state.set_r(decoded_inst.get_rd(), decoded_inst.get_imm());

    goto_next_sequential_pc(cpu_state);
}

void execute::branch(const decoded_inst_t& decoded_inst, cpu_state_t& cpu_state) {
    irvelog(2, "Executing BRANCH instruction");

    assert((decoded_inst.get_opcode() == BRANCH) && "branch instruction must have opcode BRANCH");
    assert((decoded_inst.get_format() == B_TYPE) && "branch instruction must be B_TYPE");

    // Get operands
    reg_t r1 = cpu_state.get_r(decoded_inst.get_rs1());
    reg_t r2 = cpu_state.get_r(decoded_inst.get_rs2());
    word_t imm = decoded_inst.get_imm();
    uint8_t funct3 = decoded_inst.get_funct3();

    bool branch{};
    switch(funct3) {
        case 0b000://BEQ
            irvelog(3, "Mnemonic: BEQ");
            branch = (r1 == r2);
            irvelog(3, "0x%08X == 0x%08X results in %X", r1.u, r2.u, branch);
            break;
        case 0b001://BNE
            irvelog(3, "Mnemonic: BNE");
            branch = (r1 != r2);
            irvelog(3, "0x%08X != 0x%08X results in %X", r1.u, r2.u, branch);
            break;
        case 0b100://BLT
            irvelog(3, "Mnemonic: BLT");
            branch = (r1.s < r2.s);
            irvelog(3, "0x%08X < 0x%08X (signed) results in %X", r1.u, r2.u, branch);
            break;
        case 0b101://BGE
            irvelog(3, "Mnemonic: BGE");
            branch = (r1.s >= r2.s);
            irvelog(3, "0x%08X >= 0x%08X (signed) results in %X", r1.u, r2.u, branch);
            break;
        case 0b110://BLTU
            irvelog(3, "Mnemonic: BLTU");
            branch = (r1.u < r2.u);
            irvelog(3, "0x%08X < 0x%08X (unsigned) results in %X", r1.u, r2.u, branch);
            break;
        case 0b111://BGEU
            irvelog(3, "Mnemonic: BGEU");
            branch = (r1.u >= r2.u);
            irvelog(3, "0x%08X >= 0x%08X (unsigned) results in %X", r1.u, r2.u, branch);
            break;
        default:
            throw rvexception_t(ILLEGAL_INSTRUCTION_EXCEPTION);
            break;
    }

    if(branch) {
        word_t target_addr = cpu_state.get_pc() + imm;
        // Target address on branches taken must be aligned on 4 byte boundary
        // (2 byte boundary if supporting compressed instructions)
        if (target_addr.u % 4) {
            assert(0 && "TODO implement instruction-address-misaligned exception");
        }
        else {
            cpu_state.set_pc(target_addr);
            irvelog(3, "Branching to 0x%08X", target_addr);
        }
    }
    else {
        goto_next_sequential_pc(cpu_state);
    }
}

void execute::jalr(const decoded_inst_t& decoded_inst, cpu_state_t& cpu_state) {
    irvelog(2, "Executing JALR instruction");

    assert((decoded_inst.get_opcode() == JALR) && "jalr instruction must have opcode JALR");
    assert((decoded_inst.get_format() == I_TYPE) && "jalr instruction must be I_TYPE");

    //TODO ensure that the immediate is aligned on a 4 byte boundary as well as the register value

    //The "link" part of jump and link
    cpu_state.set_r(decoded_inst.get_rd(), cpu_state.get_pc() + 4);

    //Jump to the address in rs1 plus the immediate
    cpu_state.set_pc(cpu_state.get_r(decoded_inst.get_rs1()).u + decoded_inst.get_imm().u);
}

void execute::jal(const decoded_inst_t& decoded_inst, cpu_state_t& cpu_state) {
    irvelog(2, "Executing JAL instruction");

    assert((decoded_inst.get_opcode() == JAL) && "jal instruction must have opcode JAL");
    assert((decoded_inst.get_format() == J_TYPE) && "jal instruction must be J_TYPE");

    //TODO ensure that the immediate is aligned on a 4 byte boundary

    //The "link" part of jump and link
    cpu_state.set_r(decoded_inst.get_rd(), cpu_state.get_pc() + 4);

    //Jump relative to the current PC
    cpu_state.set_pc(cpu_state.get_pc() + decoded_inst.get_imm().u);
}

void execute::system(const decoded_inst_t& decoded_inst, cpu_state_t& cpu_state, memory_t& memory) {
    irvelog(2, "Executing SYSTEM instruction");

    assert((decoded_inst.get_opcode() == SYSTEM) && "system instruction must have opcode SYSTEM");
    // assert((decoded_inst.get_format() == I_TYPE) && "system instruction must be I_TYPE"); //TODO SYSTEM can also be R-Type
    
    //TODO also handle supervisor mode instructions

    // Get operands
    reg_t rs1 = cpu_state.get_r(decoded_inst.get_rs1());
    reg_t rs2 = cpu_state.get_r(decoded_inst.get_rs2());
    uint8_t funct7 = decoded_inst.get_funct7();
    word_t imm = decoded_inst.get_imm();
    privilege_mode_t privilege_mode = cpu_state.get_privilege_mode();

    switch (decoded_inst.get_funct3()) {
        case 0b000://ECALL, EBREAK, WFI, MRET, or SRET
            //For all of these, the register fields rd and rs1 must be zero
            if (decoded_inst.get_rs1() != 0 || decoded_inst.get_rd() != 0) {
                throw rvexception_t(ILLEGAL_INSTRUCTION_EXCEPTION);
            }

            if (imm == 0b000000000000) {//ECALL
                irvelog(3, "Mnemonic: ECALL");
                //TODO write any CSRs that need to be written
                switch (cpu_state.get_privilege_mode()) {
                    case privilege_mode_t::MACHINE_MODE:
                        irvelog(4, "Executing ECALL from Machine Mode");
                        cpu_state.set_CSR(MEPC_ADDRESS, cpu_state.get_pc());//NOT the next instruction's PC
                        throw rvexception_t(MMODE_ECALL_EXCEPTION);
                        break;
                    case privilege_mode_t::SUPERVISOR_MODE:
                        irvelog(4, "Privilege Mode: Supervisor Mode");
                        throw rvexception_t(SMODE_ECALL_EXCEPTION);
                        break;
                    case privilege_mode_t::USER_MODE:
                        irvelog(4, "Privilege Mode: User Mode");
                        throw rvexception_t(UMODE_ECALL_EXCEPTION);
                        break;
                    default:
                        assert(false && "Currently in invalid privilege mode, this should never happen");
                        break;
                }
            } else if (imm == 0b00000000001) {//EBREAK
                irvelog(3, "Mnemonic: EBREAK");
                throw rvexception_t(BREAKPOINT_EXCEPTION);
            } else if (imm == 0b000100000010) {//WFI//FIXME techincally this is a funct7 plus rs2, but this does work
                irvelog(3, "Mnemonic: WFI");
                irvelog(4, "It is legal \"to simply implement WFI as a NOP\", so we will do that");
            } else if ((funct7 == 0b0011000) && (decoded_inst.get_rs2() == 0b00010)) {//MRET
                irvelog(3, "Mnemonic: MRET");
                //FIXME this assumes mepc contains a physical address, but it could be a virtual address if it is from Supervisor or User mode
                //TODO better logging
                cpu_state.set_pc(cpu_state.get_CSR(MEPC_ADDRESS) & 0xFFFFFFFC);
                goto_next_sequential_pc(cpu_state);//TODO is this correct?
            } else if ((funct7 == 0b0001000) && (decoded_inst.get_rs2() == 0b00010)) {//SRET
                irvelog(3, "Mnemonic: SRET");
                assert(false && "TODO implement SRET");
            } else {
                throw rvexception_t(ILLEGAL_INSTRUCTION_EXCEPTION);
            }
            break;
        case 0b001://CSRRW
            irvelog(3, "Mnemonic: CSRRW");
            assert(false && "TODO implement CSRRW");
            break;
        case 0b010://CSRRS
            irvelog(3, "Mnemonic: CSRRS");
            {//TODO better code reuse w/ the other CSR instructions
             //TODO better logging
                reg_t csr = cpu_state.get_CSR(imm.u);
                cpu_state.set_r(decoded_inst.get_rd(), csr);
                csr |= rs1;
                cpu_state.set_CSR(imm.u, csr);
                goto_next_sequential_pc(cpu_state);
            }
            break;
        case 0b011://CSRRC
            irvelog(3, "Mnemonic: CSRRC");
            assert(false && "TODO implement CSRRC");
            break;
        case 0b101://CSRRWI
            irvelog(3, "Mnemonic: CSRRWI");
            assert(false && "TODO implement CSRRWI");
            break;
        case 0b110://CSRRSI
            irvelog(3, "Mnemonic: CSRRSI");
            assert(false && "TODO implement CSRRSI");
            break;
        case 0b111://CSRRCI
            irvelog(3, "Mnemonic: CSRRCI");
            assert(false && "TODO implement CSRRCI");
            break;
        default:
            throw rvexception_t(ILLEGAL_INSTRUCTION_EXCEPTION);
            break;
    }
}

/* Static Function Implementations */

static void goto_next_sequential_pc(cpu_state_t& cpu_state) {
    cpu_state.set_pc(cpu_state.get_pc() + 4);
    irvelog(3, "Going to next sequential PC: 0x%08X", cpu_state.get_pc()); 
}
