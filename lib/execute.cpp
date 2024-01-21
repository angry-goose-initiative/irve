/**
 * @brief   Utility functions for executing instructions
 * 
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  Copyright (C) 2023 Nick Chan\n
 *  See the LICENSE file at the root of the project for licensing info.
 *
 * Some inspiration from rv32esim
*/

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include "execute.h"

#include <cassert>
#include <cstdint>

#include "common.h"
#include "CSR.h"
#include "cpu_state.h"
#include "decode.h"
#include "rvexception.h"

#define INST_COUNT CSR.implicit_read(Csr::Address::kMinstret).u
#include "logging.h"

using namespace irve::internal;

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

void execute::load(const decode::decoded_inst_t& decoded_inst, CpuState& cpu_state,
                    memory::memory_t& memory, const Csr& CSR) {
    irvelog(2, "Executing LOAD instruction");

    assert(
        (decoded_inst.get_opcode() == decode::opcode_t::LOAD) &&
        "load instruction must have opcode LOAD"
    );
    assert(
        (decoded_inst.get_format() == decode::inst_format_t::I_TYPE) &&
        "load instruction must be I_TYPE"
    );

    // Get operands
    reg_t r1 = cpu_state.get_r(decoded_inst.get_rs1());
    reg_t imm = decoded_inst.get_imm();
    uint8_t funct3 = decoded_inst.get_funct3();

    switch (funct3) {
        case DT_SIGNED_BYTE://LB
            irvelog(3, "Mnemonic: LB");
            break;
        case DT_SIGNED_HALFWORD://LH
            irvelog(3, "Mnemonic: LH");
            break;
        case DT_WORD://LW
            irvelog(3, "Mnemonic: LW");
            break;
        case DT_UNSIGNED_BYTE://LBU
            irvelog(3, "Mnemonic: LBU");
            break;
        case DT_UNSIGNED_HALFWORD://LHU
            irvelog(3, "Mnemonic: LHU");
            break;
        default:
            invoke_rv_exception(ILLEGAL_INSTRUCTION);
            break;
    }
    //This could cause an exception
    Word loaded = memory.load(r1 + imm, funct3);

    irvelog(3, "Loaded 0x%08X from 0x%08X", loaded.s, (r1 + imm).u);
    cpu_state.set_r(decoded_inst.get_rd(), loaded);

    //Increment PC
    cpu_state.goto_next_sequential_pc();
}

void execute::custom_0(const decode::decoded_inst_t& decoded_inst, CpuState& /* cpu_state */,
                        memory::memory_t& /* memory */, Csr& CSR) {
    irvelog(2, "Executing custom-0 instruction");

    assert(
        (decoded_inst.get_opcode() == decode::opcode_t::CUSTOM_0) &&
        "custom-0 instruction must have opcode CUSTOM_0"
    );
    assert(
        (decoded_inst.get_format() == decode::inst_format_t::R_TYPE) &&
        "custom-0 instruction must be R_TYPE"
    );

    //All other fields being zero means emulator exit request
    if (!decoded_inst.get_rd() && !decoded_inst.get_funct3() && !decoded_inst.get_rs1() &&
        !decoded_inst.get_rs2() && !decoded_inst.get_funct7()) {
        irvelog(3, "Mnemonic: IRVE.EXIT");
        if (CSR.get_privilege_mode() == PrivilegeMode::MACHINE_MODE) {
            irvelog(3, "In machine mode, so the IRVE.EXIT instruction is valid");
            invoke_polite_irve_exit_request();
        }
        else {
            irvelog(
                3,
                "The IRVE.EXIT instruction is only valid in machine mode; treating as an illegal "
                "instruction"
            );
            invoke_rv_exception(ILLEGAL_INSTRUCTION);
        }
    }
    else {//Otherwise we don't implement any others for now
        invoke_rv_exception(ILLEGAL_INSTRUCTION);
    }
}

void execute::misc_mem(const decode::decoded_inst_t& decoded_inst, CpuState& cpu_state,
                        const Csr& CSR) {
    irvelog(2, "Executing MISC-MEM instruction");

    if (decoded_inst.get_funct3() == 0b000) {//FENCE
        irvelog(3, "Mnemonic: FENCE");
    } else if (decoded_inst.get_funct3() == 0b001) {//FENCE.I
        irvelog(3, "Mnemonic: FENCE.I");
    } else {
        invoke_rv_exception(ILLEGAL_INSTRUCTION);
    }

    irvelog(3, "Nothing to do since the emulated system dosn't have a cache or multiple harts");

    //Increment PC
    cpu_state.goto_next_sequential_pc();
}

void execute::op_imm(const decode::decoded_inst_t& decoded_inst, CpuState& cpu_state,
                        const Csr& CSR) {
    irvelog(2, "Executing OP-IMM instruction");

    assert(
        (decoded_inst.get_opcode() == decode::opcode_t::OP_IMM) &&
        "op_imm() instruction must have opcode OP-IMM"
    );
    assert(
        (decoded_inst.get_format() == decode::inst_format_t::I_TYPE) &&
        "op_imm() instruction must be I_TYPE"
    );

    //Get operands
    reg_t r1 = cpu_state.get_r(decoded_inst.get_rs1());
    Word imm = decoded_inst.get_imm();
    uint8_t funct7 = imm.bits(11, 5).u;

    //Perform the ALU operation
    Word result;
    switch (decoded_inst.get_funct3()) {
        case 0b000://ADDI
            irvelog(3, "Mnemonic: ADDI");
            result = r1 + imm;
            irvelog(3, "0x%08X + 0x%08X = 0x%08X", r1.u, imm.u, result.u);
            break;
        case 0b001://SLLI
            irvelog(3, "Mnemonic: SLLI");
            result = r1 << imm.bits(4, 0);
            irvelog(3, "0x%08X << 0x%08X = 0x%08X", r1.u, imm.u, result.u);
            break;
        case 0b010://SLTI
            irvelog(3, "Mnemonic: SLTI");
            result = (r1.s < imm.s) ? 1 : 0;
            irvelog(3, "(0x%08X signed < 0x%08X signed) ? 1 : 0 = 0x%08X", r1.u, imm.u, result.u);
            break;
        case 0b011://SLTIU
            irvelog(3, "Mnemonic: SLTIU");
            result = (r1.u < imm.u) ? 1 : 0;
            irvelog(3, "(0x%08X unsigned < 0x%08X unsigned) ? 1 : 0 = 0x%08X", r1.u, imm.u, result.u);
            break;
        case 0b100://XORI
            irvelog(3, "Mnemonic: XORI");
            result = r1 ^ imm;
            irvelog(3, "0x%08X ^ 0x%08X = 0x%08X", r1.u, imm.u, result.u);
            break;
        case 0b101://SRLI or SRAI
            if (funct7 == 0b0000000) {//SRLI
                irvelog(3, "Mnemonic: SRLI");
                result = r1.srl(imm.bits(4, 0));
                irvelog(3, "0x%08X >> 0x%08X logical = 0x%08X", r1.u, imm.u, result.u);
            } else if (funct7 == 0b0100000) {//SRAI
                irvelog(3, "Mnemonic: SRAI");
                result = r1.sra(imm.bits(4, 0));
                irvelog(3, "0x%08X >> 0x%08X arithmetic = 0x%08X", r1.u, imm.u, result.u);
            } else {
                invoke_rv_exception(ILLEGAL_INSTRUCTION);
            }
            break;
        case 0b110://ORI
            irvelog(3, "Mnemonic: ORI");
            result = r1 | imm;
            irvelog(3, "0x%08X | 0x%08X = 0x%08X", r1.u, imm.u, result.u);
            break;
        case 0b111://ANDI
            irvelog(3, "Mnemonic: ANDI");
            result = r1 & imm;
            irvelog(3, "0x%08X & 0x%08X = 0x%08X", r1.u, imm.u, result.u);
            break;
        default:
            assert(false && "We should never get here");
            break;
    }
    irvelog(3, "Overwriting 0x%08X currently in register x%u with 0x%08X",
            cpu_state.get_r(decoded_inst.get_rd()).u, decoded_inst.get_rd(), result);
    cpu_state.set_r(decoded_inst.get_rd(), result.u);

    //Increment PC
    cpu_state.goto_next_sequential_pc();
}

void execute::auipc(const decode::decoded_inst_t& decoded_inst, CpuState& cpu_state,
                    const Csr& CSR) {
    irvelog(2, "Executing AUIPC instruction");

    assert(
        (decoded_inst.get_opcode() == decode::opcode_t::AUIPC) &&
        "auipc instruction must have opcode AUIPC"
    );
    assert(
        (decoded_inst.get_format() == decode::inst_format_t::U_TYPE) &&
        "auipc instruction must be U_TYPE"
    );

    Word result = decoded_inst.get_imm() + cpu_state.get_pc();

    irvelog(3, "Overwriting 0x%08X currently in register x%u with 0x%08X",
            cpu_state.get_r(decoded_inst.get_rd()).u, decoded_inst.get_rd(), result.u);
    cpu_state.set_r(decoded_inst.get_rd(), result);

    cpu_state.goto_next_sequential_pc();
}

void execute::store(const decode::decoded_inst_t& decoded_inst, CpuState& cpu_state,
                    memory::memory_t& memory, const Csr& CSR) {
    irvelog(2, "Executing STORE instruction");

    assert(
        (decoded_inst.get_opcode() == decode::opcode_t::STORE) &&
        "store instruction must have opcode STORE"
    );
    assert(
        (decoded_inst.get_format() == decode::inst_format_t::S_TYPE) &&
        "store instruction must be S_TYPE"
    );

    // Get operands
    reg_t r1 = cpu_state.get_r(decoded_inst.get_rs1());
    reg_t r2 = cpu_state.get_r(decoded_inst.get_rs2());
    Word imm = decoded_inst.get_imm();
    uint8_t funct3 = decoded_inst.get_funct3();

    switch(funct3) {
        case DT_BYTE://SB
            irvelog(3, "Mnemonic: SB");
            break;
        case DT_HALFWORD://SH
            irvelog(3, "Mnemonic: SH");
            break;
        case DT_WORD://SW
            irvelog(3, "Mnemonic: SW");
            break;
        default:
            invoke_rv_exception(ILLEGAL_INSTRUCTION);
            break;
    }
    
    irvelog(3, "Storing 0x%08X in 0x%08X", r2.u, r1.u + imm.u);

    //This could raise an exception
    memory.store(r1.u + imm.u, funct3, r2.s);

    //Increment PC
    cpu_state.goto_next_sequential_pc();
}

void execute::amo(const decode::decoded_inst_t& decoded_inst, CpuState& cpu_state,
                    memory::memory_t& memory, const Csr& CSR) {
    irvelog(2, "Executing AMO instruction");

    //TODO Vol 2 Page 80 comments on AMO exceptions wrt. virtual memory, it may be relevant

    //Sanity checks
    assert((decoded_inst.get_opcode() == decode::opcode_t::AMO) && 
            "amo instruction must have opcode AMO");
    assert((decoded_inst.get_format() == decode::inst_format_t::R_TYPE) &&
            "amo instruction must be R_TYPE");
    
    if (decoded_inst.get_funct3() != 0b010) {
        invoke_rv_exception(ILLEGAL_INSTRUCTION);
    }
    //NOTE: All possible aq and rl values are valid, so we don't need to check them

    //Get operands
    reg_t r1 = cpu_state.get_r(decoded_inst.get_rs1());
    reg_t r2 = cpu_state.get_r(decoded_inst.get_rs2());

    Word loaded_word;
    switch (decoded_inst.get_funct5()) {
        case 0b00010://LR.W
            irvelog(3, "Mnemonic: LR.W");

            //Check that the address is word-aligned
            if ((r1.u % 4) != 0) {
                // NOTE: This exception has priority over access faults but not over the illegal
                // instruction exception. This is why we don't do this before the switch statement.
                invoke_rv_exception(STORE_OR_AMO_ADDRESS_MISALIGNED);
            }
            
            //Load the word from memory at the address in rs1
            try {
                loaded_word = memory.load(r1, DT_WORD);
            } catch (const rvexception::rvexception_t& e) {
                // If we get an exception, we need to rethrow a different one to indicate this is
                // due to an AMO instruction
                switch (e.cause()) {//TODO ensure this is correct
                    case rvexception::cause_t::LOAD_ADDRESS_MISALIGNED_EXCEPTION:
                        assert(
                            false &&
                            "Got a misaligned address exception when reading from memory, but we "
                            "already checked that the address was aligned!"
                        );
                        break;
                    case rvexception::cause_t::LOAD_ACCESS_FAULT_EXCEPTION:
                        invoke_rv_exception(STORE_OR_AMO_ACCESS_FAULT);
                        break;
                    case rvexception::cause_t::LOAD_PAGE_FAULT_EXCEPTION:
                        invoke_rv_exception(STORE_OR_AMO_PAGE_FAULT);
                        break;
                    default:
                        assert(false && "Unexpected exception when reading from memory");
                        break;
                }
            }

            //Save it into rd
            cpu_state.set_r(decoded_inst.get_rd(), loaded_word);

            //If we get here, the load was successful; the "reservation set" is valid
            //It will stay valid until an exception occurs or a SC.W instruction is executed
            cpu_state.validate_reservation_set();

            cpu_state.goto_next_sequential_pc();
            return;
        case 0b00011://SC.W
            irvelog(3, "Mnemonic: SC.W");

            //Check that the address is word-aligned
            //TODO should we only check alignment if the reservation set is valid?
            if ((r1.u % 4) != 0) {
                // NOTE: This exception has priority over access faults but not over the illegal
                // instruction exception. This is why we don't do this before the switch statement.
                invoke_rv_exception(STORE_OR_AMO_ADDRESS_MISALIGNED);
            }

            //Check if the reservation set is valid
            if (!cpu_state.reservation_set_valid()) {
                //If not, write a non-zero value to rd and go to the next instruction
                cpu_state.set_r(decoded_inst.get_rd(), 1);
                cpu_state.goto_next_sequential_pc();
                return;
            }
            
            //If we get here, the reservation set is valid
            //We are now performing the store, so the reservation set is no longer valid
            cpu_state.invalidate_reservation_set();

            //Attempt to store the value in rs2 to the address in rs1
            try {
                memory.store(r1, DT_WORD, r2);
            } catch (const rvexception::rvexception_t& e) {
                // If we get an exception, we need to rethrow a different one to indicate this is
                // due to an AMO instruction
                switch (e.cause()) {//TODO ensure this is correct
                    case rvexception::cause_t::LOAD_ADDRESS_MISALIGNED_EXCEPTION:
                        assert(
                            false &&
                            "Got a misaligned address exception when reading from memory, but we "
                            " already checked that the address was aligned!"
                        );
                        break;
                    case rvexception::cause_t::LOAD_ACCESS_FAULT_EXCEPTION:
                        invoke_rv_exception(STORE_OR_AMO_ACCESS_FAULT);
                        break;
                    case rvexception::cause_t::LOAD_PAGE_FAULT_EXCEPTION:
                        invoke_rv_exception(STORE_OR_AMO_PAGE_FAULT);
                        break;
                    default:
                        assert(false && "Unexpected exception when reading from memory");
                        break;
                }
            }

            //If we get here, the store was successful; write 0 to rd
            cpu_state.set_r(decoded_inst.get_rd(), 0);

            //And we're done!
            cpu_state.goto_next_sequential_pc();
            return;
        case 0b00001://AMOSWAP.W
            irvelog(3, "Mnemonic: AMOSWAP.W");
            break;
        case 0b00000://AMOADD.W
            irvelog(3, "Mnemonic: AMOADD.W");
            break;
        case 0b00100://AMOXOR.W
            irvelog(3, "Mnemonic: AMOXOR.W");
            break;
        case 0b01100://AMOAND.W
            irvelog(3, "Mnemonic: AMOAND.W");
            break;
        case 0b01000://AMOOR.W
            irvelog(3, "Mnemonic: AMOOR.W");
            break;
        case 0b10000://AMOMIN.W
            irvelog(3, "Mnemonic: AMOMIN.W");
            break;
        case 0b10100://AMOMAX.W
            irvelog(3, "Mnemonic: AMOMAX.W");
            break;
        case 0b11000://AMOMINU.W
            irvelog(3, "Mnemonic: AMOMINU.W");
            break;
        case 0b11100://AMOMAXU.W
            irvelog(3, "Mnemonic: AMOMAXU.W");
            break;
        default:
            invoke_rv_exception(ILLEGAL_INSTRUCTION);
            break;
    }

    //If we got here, this is a "proper" AMO instruction (i.e. not LR.W or SC.W)

    //Check that the address is word-aligned
    if ((r1.u % 4) != 0) {
        // NOTE: This exception has priority over access faults but not over the illegal
        // instruction exception. This is why we don't do this before the switch statement.
        invoke_rv_exception(STORE_OR_AMO_ADDRESS_MISALIGNED);
    }

    //Read the word at the address in rs1
    try {
        loaded_word = memory.load(r1, DT_WORD);
    } catch (const rvexception::rvexception_t& e) {
        // If we get an exception, we need to rethrow a different one to indicate this is due to an
        // AMO instruction
        switch (e.cause()) {//TODO ensure this is correct
            case rvexception::cause_t::LOAD_ADDRESS_MISALIGNED_EXCEPTION:
                assert(
                    false &&
                    "Got a misaligned address exception when reading from memory, but we already "
                    "checked that the address was aligned!"
                );
                break;
            case rvexception::cause_t::LOAD_ACCESS_FAULT_EXCEPTION:
                invoke_rv_exception(STORE_OR_AMO_ACCESS_FAULT);
                break;
            case rvexception::cause_t::LOAD_PAGE_FAULT_EXCEPTION:
                invoke_rv_exception(STORE_OR_AMO_PAGE_FAULT);
                break;
            default:
                assert(false && "Unexpected exception when reading from memory");
                break;
        }
    }

    //Save it into rd
    cpu_state.set_r(decoded_inst.get_rd(), loaded_word);

    //Perform the operation (instruction-specific)
    Word Wordo_write;
    switch (decoded_inst.get_funct5()) {
        case 0b00001://AMOSWAP.W
            Wordo_write = r2;
            break;
        case 0b00000://AMOADD.W
            Wordo_write = loaded_word + r2;
            break;
        case 0b00100://AMOXOR.W
            Wordo_write = loaded_word ^ r2;
            break;
        case 0b01100://AMOAND.W
            Wordo_write = loaded_word & r2;
            break;
        case 0b01000://AMOOR.W
            Wordo_write = loaded_word | r2;
            break;
        case 0b10000://AMOMIN.W
            Wordo_write = std::min(loaded_word.s, r2.s);
            break;
        case 0b10100://AMOMAX.W
            Wordo_write = std::max(loaded_word.s, r2.s);
            break;
        case 0b11000://AMOMINU.W
            Wordo_write = std::min(loaded_word.u, r2.u);
            break;
        case 0b11100://AMOMAXU.W
            Wordo_write = std::max(loaded_word.u, r2.u);
            break;
        default:
            assert(false && "Invalid funct5 for AMO instruction, but we already checked this!");
            break;
    }
    memory.store(r1, DT_WORD, Wordo_write);

    cpu_state.goto_next_sequential_pc();
}

void execute::op(const decode::decoded_inst_t& decoded_inst, CpuState& cpu_state,
                    const Csr& CSR) {
    irvelog(2, "Executing OP instruction"); 

    assert(
        (decoded_inst.get_opcode() == decode::opcode_t::OP) &&
        "op instruction must have opcode OP"
    );
    assert(
        (decoded_inst.get_format() == decode::inst_format_t::R_TYPE) &&
        "op instruction must be R_TYPE"
    );

    //Get operands
    reg_t r1 = cpu_state.get_r(decoded_inst.get_rs1());
    reg_t r2 = cpu_state.get_r(decoded_inst.get_rs2());

    //Perform the ALU operation
    Word result;
    if (decoded_inst.get_funct7() == 0b0000001) {//M extension instructions
        switch (decoded_inst.get_funct3()) {
            case 0b000://MUL
                irvelog(3, "Mnemonic: MUL");

                result = r1 * r2;
                irvelog(3, "0x%08X * 0x%08X = 0x%08X", r1.u, r2.u, result);

                break;
            case 0b001://MULH
                irvelog(3, "Mnemonic: MULH");

                //TODO ensure casting to int64_t actually performs sign extension
                result = (uint32_t)((((int64_t)r1.s) * ((int64_t)r2.s)) >> 32);

                irvelog(
                    3,
                    "0x%08X signed * 0x%08X signed upper half = 0x%08X",
                    r1.s, r2.s, result
                );
                break;
            case 0b010://MULHSU
                irvelog(3, "Mnemonic: MULHSU");

                //TODO ensure casting to int64_t actually performs sign extension ONLY WHEN CASTING rs1.s since it is signed
                result = (uint32_t)((((int64_t)r1.s) * ((int64_t)((uint64_t)r2.u))) >> 32);

                irvelog(
                    3,
                    "0x%08X signed * 0x%08X unsigned upper half = 0x%08X",
                    r1.s, r2.u, result
                );
                break;
            case 0b011://MULHU
                irvelog(3, "Mnemonic: MULHU");

                result = (uint32_t)((((uint64_t)r1.u) * ((uint64_t)r2.u)) >> 32);

                irvelog(
                    3,
                    "0x%08X unsigned * 0x%08X unsigned upper half = 0x%08X",
                    r1.u, r2.u, result
                );
                break;
            case 0b100://DIV
                irvelog(3, "Mnemonic: DIV");

                if (!r2) {//Division by zero
                    result = 0xFFFFFFFF;
                }
                else if ((r1 == 0x80000000) && (r2 == -1)) {
                    // Overflow (division of the most negative number by -1)
                    result = 0x80000000;
                }
                else {
                    result = r1.s / r2.s;
                }

                irvelog(3, "0x%08X signed / 0x%08X signed = 0x%08X", r1.s, r2.s, result);
                break;
            case 0b101://DIVU
                irvelog(3, "Mnemonic: DIVU");

                if (!r2) {//Division by zero
                    result = 0xFFFFFFFF;
                }
                else {
                    result = r1.u / r2.u;
                }

                irvelog(3, "0x%08X unsigned / 0x%08X unsigned = 0x%08X", r1.u, r2.u, result);
                break;
            case 0b110://REM
                irvelog(3, "Mnemonic: REM");

                if (!r2) {//Division by zero
                    result = r1;
                }
                else if ((r1 == 0x80000000) && (r2 == -1)) {
                    // Overflow (division of the most negative number by -1)
                    result = 0;
                }
                else {
                    result = r1.s % r2.s;
                }

                irvelog(3, "0x%08X signed %% 0x%08X signed = 0x%08X", r1.s, r2.s, result);
                break;
            case 0b111://REMU
                irvelog(3, "Mnemonic: REMU");

                if (!r2) {//Division by zero
                    result = r1;
                }
                else {
                    result = r1.u % r2.u;
                }

                irvelog(3, "0x%08X unsigned %% 0x%08X unsigned = 0x%08X", r1.u, r2.u, result);
                break;
            default:
                assert(false && "We should never get here");
                break;
        }
    }
    else {//Others (base spec)
        switch (decoded_inst.get_funct3()) {
            case 0b000://ADD or SUB
                if (decoded_inst.get_funct7() == 0b0000000) {//ADD
                    irvelog(3, "Mnemonic: ADD");
                    result = r1 + r2;
                    irvelog(3, "0x%08X + 0x%08X = 0x%08X", r1.u, r2.u, result);
                }
                else if (decoded_inst.get_funct7() == 0b0100000) {//SUB
                    irvelog(3, "Mnemonic: SUB");
                    result = r1 - r2;
                    irvelog(3, "0x%08X - 0x%08X = 0x%08X", r1.u, r2.u, result);
                }
                else {
                    invoke_rv_exception(ILLEGAL_INSTRUCTION);
                }
                break;
            case 0b001://SLL
                irvelog(3, "Mnemonic: SLL");

                if (decoded_inst.get_funct7() != 0b0000000) {
                    invoke_rv_exception(ILLEGAL_INSTRUCTION);
                }

                result = r1 << r2.bits(4, 0);

                irvelog(3, "0x%08X << 0x%08X logical = 0x%08X", r1.u, r2.u, result);
                break;
            case 0b010://SLT
                irvelog(3, "Mnemonic: SLT");

                if (decoded_inst.get_funct7() != 0b0000000) {
                    invoke_rv_exception(ILLEGAL_INSTRUCTION);
                }

                result = (r1.s < r2.s) ? 1 : 0;

                irvelog(3, "(0x%08X signed < 0x%08X signed) = 0x%08X", r1.u, r2.u, result);
                break;
            case 0b011://SLTU
                irvelog(3, "Mnemonic: SLTU");

                if (decoded_inst.get_funct7() != 0b0000000) {
                    invoke_rv_exception(ILLEGAL_INSTRUCTION);
                }

                result = (r1.u < r2.u) ? 1 : 0;

                irvelog(3, "(0x%08X unsigned < 0x%08X unsigned) = 0x%08X", r1.u, r2, result);
                break;
            case 0b100://XOR
                irvelog(3, "Mnemonic: XOR");

                if (decoded_inst.get_funct7() != 0b0000000) {
                    invoke_rv_exception(ILLEGAL_INSTRUCTION);
                }

                result = r1 ^ r2;

                irvelog(3, "0x%08X ^ 0x%08X = 0x%08X", r1.u, r2.u, result);
                break;
            case 0b101://SRL or SRA
                if (decoded_inst.get_funct7() == 0b0000000) {//SRL
                    irvelog(3, "Mnemonic: SRL");
                    result = r1.srl(r2.bits(4, 0));
                    irvelog(3, "0x%08X >> 0x%08X logical = 0x%08X", r1.u, r2.u, result.u);
                }
                else if (decoded_inst.get_funct7() == 0b0100000) {//SRA
                    irvelog(3, "Mnemonic: SRA");
                    result = r1.sra(r2.bits(4, 0));
                    irvelog(3, "0x%08X >> 0x%08X arithmetic = 0x%08X", r1.u, r2.u, result.u);
                }
                else {
                    invoke_rv_exception(ILLEGAL_INSTRUCTION);
                }
                break;
            case 0b110://OR
                irvelog(3, "Mnemonic: OR");

                if (decoded_inst.get_funct7() != 0b0000000) {
                    invoke_rv_exception(ILLEGAL_INSTRUCTION);
                }

                result = r1 | r2;

                irvelog(3, "0x%08X | 0x%08X = 0x%08X", r1.u, r2.u, result);
                break;
            case 0b111://AND
                irvelog(3, "Mnemonic: AND");

                if (decoded_inst.get_funct7() != 0b0000000) {
                    invoke_rv_exception(ILLEGAL_INSTRUCTION);
                }

                result = r1 & r2;

                irvelog(3, "0x%08X & 0x%08X = 0x%08X", r1.u, r2.u, result);
                break;
            default:
                assert(false && "We should never get here");
                break;
        }
    }
    irvelog(3, "Overwriting 0x%08X currently in register x%u with 0x%08X",
            cpu_state.get_r(decoded_inst.get_rd()).u, decoded_inst.get_rd(), result);
    cpu_state.set_r(decoded_inst.get_rd(), result.u);

    cpu_state.goto_next_sequential_pc();
}

void execute::lui(const decode::decoded_inst_t& decoded_inst, CpuState& cpu_state,
                    const Csr& CSR) {
    irvelog(2, "Executing LUI instruction");

    assert(
        (decoded_inst.get_opcode() == decode::opcode_t::LUI) &&
        "lui instruction must have opcode LUI"
    );
    assert(
        (decoded_inst.get_format() == decode::inst_format_t::U_TYPE) &&
        "lui instruction must be U_TYPE"
    );

    irvelog(
        3,
        "Overwriting 0x%08X currently in register x%u with 0x%08X",
        cpu_state.get_r(decoded_inst.get_rd()).u, decoded_inst.get_rd(), decoded_inst.get_imm()
    );
    cpu_state.set_r(decoded_inst.get_rd(), decoded_inst.get_imm());

    cpu_state.goto_next_sequential_pc();
}

void execute::branch(const decode::decoded_inst_t& decoded_inst, CpuState& cpu_state,
                        const Csr& CSR) {
    irvelog(2, "Executing BRANCH instruction");

    assert(
        (decoded_inst.get_opcode() == decode::opcode_t::BRANCH) &&
        "branch instruction must have opcode BRANCH"
    );
    assert(
        (decoded_inst.get_format() == decode::inst_format_t::B_TYPE) &&
        "branch instruction must be B_TYPE"
    );

    // Get operands
    reg_t r1 = cpu_state.get_r(decoded_inst.get_rs1());
    reg_t r2 = cpu_state.get_r(decoded_inst.get_rs2());
    Word imm = decoded_inst.get_imm();
    uint8_t funct3 = decoded_inst.get_funct3();

    bool branch;
    switch (funct3) {
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
            invoke_rv_exception(ILLEGAL_INSTRUCTION);
            break;
    }

    if (branch) {
        Word target_addr = cpu_state.get_pc() + imm;
        // Target address on branches taken must be aligned on 4 byte boundary
        // (2 byte boundary if supporting compressed instructions)
        if (target_addr.u % 4) {
            invoke_rv_exception(INSTRUCTION_ADDRESS_MISALIGNED);
        }
        else {
            cpu_state.set_pc(target_addr);
            irvelog(3, "Branching to 0x%08X", target_addr);
        }
    }
    else {
        cpu_state.goto_next_sequential_pc();
    }
}

void execute::jalr(const decode::decoded_inst_t& decoded_inst, CpuState& cpu_state,
                    const Csr& CSR) {
    irvelog(2, "Executing JALR instruction");

    assert(
        (decoded_inst.get_opcode() == decode::opcode_t::JALR) &&
        "jalr instruction must have opcode JALR"
    );
    assert(
        (decoded_inst.get_format() == decode::inst_format_t::I_TYPE) &&
        "jalr instruction must be I_TYPE"
    );

    //TODO ensure that the immediate is aligned on a 4 byte boundary as well as the register value

    Word old_pc = cpu_state.get_pc();

    //Jump to the address in rs1 plus the immediate
    Word rs1 = cpu_state.get_r(decoded_inst.get_rs1());
    Word imm = decoded_inst.get_imm();
    Word destination_pc = rs1 + imm;
    irvelog(3, "0x%08X + 0x%08X = 0x%08X", rs1.u, imm.u, destination_pc);
    cpu_state.set_pc(destination_pc);

    //The "link" part of jump and link
    //Critically this must be done after the jump to the destination_pc to avoid clobbering the
    //register before it is used
    cpu_state.set_r(decoded_inst.get_rd(), old_pc + 4);//Critically we use old_pc here
}

void execute::jal(const decode::decoded_inst_t& decoded_inst, CpuState& cpu_state,
                    const Csr& CSR) {
    irvelog(2, "Executing JAL instruction");

    assert(
        (decoded_inst.get_opcode() == decode::opcode_t::JAL) &&
        "jal instruction must have opcode JAL"
    );
    assert(
        (decoded_inst.get_format() == decode::inst_format_t::J_TYPE) &&
        "jal instruction must be J_TYPE"
    );

    //TODO ensure that the immediate is aligned on a 4 byte boundary

    Word old_pc = cpu_state.get_pc();

    //Jump relative to the current PC
    cpu_state.set_pc(cpu_state.get_pc() + decoded_inst.get_imm().u);

    //The "link" part of jump and link
    cpu_state.set_r(decoded_inst.get_rd(), old_pc + 4);//Critically we use old_pc here
}

void execute::system(const decode::decoded_inst_t& decoded_inst, CpuState& cpu_state,
                        Csr& CSR) {
    irvelog(2, "Executing SYSTEM instruction");

    assert(
        (decoded_inst.get_opcode() == decode::opcode_t::SYSTEM) &&
        "system instruction must have opcode SYSTEM"
    );
    // assert(
    //     (decoded_inst.get_format() == decode::inst_format_t::I_TYPE) &&
    //     "system instruction must be I_TYPE"
    // ); //TODO SYSTEM can also be R-Type
    
    //TODO also handle supervisor mode instructions

    // Get operands
    reg_t r1 = cpu_state.get_r(decoded_inst.get_rs1());
    //reg_t rs2 = cpu_state.get_r(decoded_inst.get_rs2());
    uint8_t funct7 = decoded_inst.get_funct7();
    Word imm = decoded_inst.get_imm();
    PrivilegeMode privilege_mode = CSR.get_privilege_mode();

    switch (decoded_inst.get_funct3()) {
        case 0b000://ECALL, EBREAK, WFI, MRET, SRET, or SFENCE.VMA
            //For all of these, the register fields rd and rs1 must be zero
            //FIXME this isn't correct (ex. SFENCE.VMA)
            /*if (decoded_inst.get_rs1() != 0 || decoded_inst.get_rd() != 0) {
                invoke_rv_exception(ILLEGAL_INSTRUCTION);
            }
            */

            if (imm == 0b000000000000) {//ECALL
                irvelog(3, "Mnemonic: ECALL");

                // ECALL dosn't actually retire, but we already incremented minstret, so we need to
                // decrement it to compensate
                CSR.implicit_write(
                    Csr::Address::kMinstret,
                    CSR.implicit_read(Csr::Address::kMinstret) - 1
                );

                //Different exception case base on the current privilege mode
                switch (privilege_mode) {
                    case PrivilegeMode::MACHINE_MODE:
                        irvelog(4, "Executing ECALL from Machine Mode");
                        invoke_rv_exception(MMODE_ECALL);
                        break;
                    case PrivilegeMode::SUPERVISOR_MODE:
                        irvelog(4, "Privilege Mode: Supervisor Mode");
                        invoke_rv_exception(SMODE_ECALL);
                        break;
                    case PrivilegeMode::USER_MODE:
                        irvelog(4, "Privilege Mode: User Mode");
                        invoke_rv_exception(UMODE_ECALL);
                        break;
                    default:
                        assert(
                            false &&
                            "Currently in invalid privilege mode, this should never happen"
                        );
                        break;
                }
            }
            else if (imm == 0b00000000001) {//EBREAK
                irvelog(3, "Mnemonic: EBREAK");

                // EBREAK dosn't actually retire, but we already incremented minstret, so we need
                // to decrement it to compensate
                CSR.implicit_write(
                    Csr::Address::kMinstret,
                    CSR.implicit_read(Csr::Address::kMinstret) - 1
                );

                invoke_rv_exception(BREAKPOINT);
            }
            else if (imm == 0b000100000101) {//WFI//FIXME techincally this is a funct7 plus rs2, but this does work
                irvelog(3, "Mnemonic: WFI");
                irvelog(4, "It is legal \"to simply implement WFI as a NOP\", so we will do that");
                cpu_state.goto_next_sequential_pc();
            }
            else if ((funct7 == 0b0011000) && (decoded_inst.get_rs2() == 0b00010)) {//MRET
                irvelog(3, "Mnemonic: MRET");
                //TODO better logging
                //Manage the privilege stack
                Word mstatus = CSR.implicit_read(Csr::Address::kMstatus);
                CSR.set_privilege_mode((PrivilegeMode)mstatus.bits(12, 11).u);//Set the privilege mode to the value in MPP
                Word mpie = mstatus.bit(7);
                mstatus &= 0b11111111111111111110011101110111;//Clear the MPP, and MPIE, and MIE bits
                //MPP is set to 0b00
                mstatus |= 1 << 7;//Set MPIE to 1
                mstatus |= mpie << 3;//Set MIE to the old MPIE
                CSR.implicit_write(Csr::Address::kMstatus, mstatus);//Write changes back to the CSR

                cpu_state.invalidate_reservation_set();//Could have interrupted an LR/SC sequence

                //Return to the address in MEPC
                cpu_state.set_pc(CSR.implicit_read(Csr::Address::kMepc));
                //We do NOT go to the PC after the instruction that caused the exception (PC + 4); the handler must do this manually
            }
            else if ((funct7 == 0b0001000) && (decoded_inst.get_rs2() == 0b00010)) {//SRET
                irvelog(3, "Mnemonic: SRET");
                //TODO better logging
                //Manage the privilege stack
                Word sstatus = CSR.implicit_read(Csr::Address::kSstatus);
                //Set the privilege mode based on the value in SPP
                CSR.set_privilege_mode(
                    (sstatus.bit(8) == 0b1) ? 
                        PrivilegeMode::SUPERVISOR_MODE :
                        PrivilegeMode::USER_MODE
                );
                Word spie = sstatus.bit(5);
                sstatus &= 0b11111111111111111111111011011101;//Clear the SPP, SPIE, and SIE bits
                //SPP is set to 0b0
                sstatus |= 1 << 5;//Set SPIE to 1
                sstatus |= spie << 1;//Set SIE to the old SPIE
                CSR.implicit_write(Csr::Address::kSstatus, sstatus);//Write changes back to the CSR

                cpu_state.invalidate_reservation_set();//Could have interrupted an LR/SC sequence

                //Return to the address in SEPC
                cpu_state.set_pc(CSR.implicit_read(Csr::Address::kSepc));
                //We do NOT go to the PC after the instruction that caused the exception (PC + 4);
                //the handler must do this manually
            }
            else if ((funct7 == 0b0001001) && (decoded_inst.get_rd() == 0b00000)) {//SFENCE.VMA
                irvelog(3, "Mnemonic: SFENCE.VMA");
                irvelog(4, "We don't have a TLB in IRVE, so this is a NOP");
                cpu_state.goto_next_sequential_pc();
            }
            else {
                invoke_rv_exception(ILLEGAL_INSTRUCTION);
            }
            return;
        case 0b001://CSRRW
            irvelog(3, "Mnemonic: CSRRW");
            break;
        case 0b010://CSRRS
            irvelog(3, "Mnemonic: CSRRS");
            break;
        case 0b011://CSRRC
            irvelog(3, "Mnemonic: CSRRC");
            break;
        case 0b101://CSRRWI
            irvelog(3, "Mnemonic: CSRRWI");
            break;
        case 0b110://CSRRSI
            irvelog(3, "Mnemonic: CSRRSI");
            break;
        case 0b111://CSRRCI
            irvelog(3, "Mnemonic: CSRRCI");
            break;
        default:
            invoke_rv_exception(ILLEGAL_INSTRUCTION);
            break;
    }

    //If we got here, this is a CSR instruction
    Csr::Address csr_addr = static_cast<Csr::Address>((imm & 0xFFF).u);//In this case we do NOT sign extend the immediate
    Word uimm = decoded_inst.get_rs1();//NOT sign extended (zero extended)
    
    //Read the CSR into the destination register
    //TODO avoid read side effects if destination register is x0
    reg_t csr = CSR.explicit_read(csr_addr);
    cpu_state.set_r(decoded_inst.get_rd(), csr);

    //What we write back depends on the instruction (and we may not write back at all)
    switch (decoded_inst.get_funct3()) {
        case 0b001://CSRRW
            csr = r1;//We always cause a write, even if r1 is x0
            CSR.explicit_write(csr_addr, csr);
            break;
        case 0b010://CSRRS
            if (decoded_inst.get_rs1()) {//If r1 is x0, then we do not cause a write
                csr |= r1;
                CSR.explicit_write(csr_addr, csr);
            }
            break;
        case 0b011://CSRRC
            if (decoded_inst.get_rs1()) {//If r1 is x0, then we do not cause a write
                csr &= ~r1;
                CSR.explicit_write(csr_addr, csr);
            }
            break;
        case 0b101://CSRRWI
            csr = uimm;
            CSR.explicit_write(csr_addr, csr);
            break;
        case 0b110://CSRRSI
            if (uimm != 0) {//If uimm is 0, then we do not cause a write
                csr |= uimm;
                CSR.explicit_write(csr_addr, csr);
            }
            break;
        case 0b111://CSRRCI
            if (uimm != 0) {//If uimm is 0, then we do not cause a write
                csr &= ~uimm;
                CSR.explicit_write(csr_addr, csr);
            }
            break;
        default:
            assert(
                false &&
                "We should have already caught illegal instructions, so we should never get here"
            );
            break;
    }

    cpu_state.goto_next_sequential_pc();
}
