/**
 * @brief   Code to decode RISC-V instructions
 * 
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  Copyright (C) 2023 Nick Chan\n
 *  See the LICENSE file at the root of the project for licensing info.
 * 
 * Based on code from rv32esim
*/

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include "decode.h"

#include <cassert>
#include <cstdint>
#include <string>

#include "config.h"

#include "common.h"

#include "rv_trap.h"

#define INST_COUNT inst_count
#include "logging.h"

#if IRVE_INTERNAL_CONFIG_RUST
#include "irve_disassemble.h"
#endif

using namespace irve::internal;

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

decode::DecodedInst::DecodedInst(Word instruction) :
    m_opcode((Opcode)instruction.bits(6, 2).u),
    m_funct3(instruction.bits(14, 12).u),
    m_funct5(instruction.bits(31, 27).u),
    m_funct7(instruction.bits(31, 25).u),
    m_rd    (instruction.bits(11, 7) .u),
    m_rs1   (instruction.bits(19, 15).u),
    m_rs2   (instruction.bits(24, 20).u),
    m_imm_I (instruction.bits(31, 20).sign_extend_from_bit_number(11).u),
    m_imm_S (
        (
            (instruction.bits(31, 25) << 5) | 
            instruction.bits (11, 7)
        )
        .sign_extend_from_bit_number(11).u
    ),
    m_imm_B (
        (
            (instruction.bit (31)       << 12)  |
            (instruction.bit (7)        << 11)  | 
            (instruction.bits(30, 25)   << 5)   | 
            (instruction.bits(11, 8)    << 1)   |
            0b0
        )
        .sign_extend_from_bit_number(12).u
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
        .sign_extend_from_bit_number(20).u
    )
{
    //These are defined invalid RISC-V instructions
    //In addition, we don't support compressed instructions
    if (!instruction || (instruction == 0xFFFFFFFF) || ((instruction & 0b11) != 0b11)) {
        rv_trap::invoke_exception(rv_trap::Cause::ILLEGAL_INSTRUCTION_EXCEPTION);
    }

    switch (this->m_opcode) {
        //R-type
        case Opcode::OP:
        case Opcode::CUSTOM_0://We implement this opcode with some custom instructions!
        case Opcode::AMO:
            this->m_format = InstFormat::R_TYPE;
            break;
        //I-type
        case Opcode::LOAD:
        case Opcode::OP_IMM:
        case Opcode::JALR:
        case Opcode::SYSTEM:
        case Opcode::MISC_MEM:
            this->m_format = InstFormat::I_TYPE;
            break;
        //S-type
        case Opcode::STORE:
            this->m_format = InstFormat::S_TYPE;
            break;
        //B-type
        case Opcode::BRANCH:
            this->m_format = InstFormat::B_TYPE;
            break;
        //U-type
        case Opcode::LUI:
        case Opcode::AUIPC:
            this->m_format = InstFormat::U_TYPE;
            break;
        //J-type
        case Opcode::JAL:
            this->m_format = InstFormat::J_TYPE;
            break;
        default:
            rv_trap::invoke_exception(rv_trap::Cause::ILLEGAL_INSTRUCTION_EXCEPTION);
            break;
    }
}

void decode::DecodedInst::log([[maybe_unused]] uint8_t indent, [[maybe_unused]] uint64_t inst_count) const {
    switch (this->get_format()) {
        case InstFormat::R_TYPE:
#if IRVE_INTERNAL_CONFIG_RUST
            irvelog(indent, "pretty = %s", this->disassemble().c_str());
#endif
            irvelog(indent, "type   = R");
            irvelog(indent, "opcode = 0x%X", this->get_opcode());
            irvelog(indent, "funct3 = 0x%X", this->get_funct3());
            irvelog(indent, "funct7 = 0x%X", this->get_funct7());
            irvelog(indent, "rd     = x%u", this->get_rd());
            irvelog(indent, "rs1    = x%u", this->get_rs1());
            irvelog(indent, "rs2    = x%u", this->get_rs2());
            break;
        case InstFormat::I_TYPE:
#if IRVE_INTERNAL_CONFIG_RUST
            irvelog(indent, "pretty = %s", this->disassemble().c_str());
#endif
            irvelog(indent, "type   = I");
            irvelog(indent, "opcode = 0x%X", this->get_opcode());
            irvelog(indent, "funct3 = 0x%X", this->get_funct3());
            irvelog(indent, "rd     = x%u", this->get_rd());
            irvelog(indent, "rs1    = x%u", this->get_rs1());
            irvelog(indent, "imm    = 0x%X", this->get_imm());
            break;
        case InstFormat::S_TYPE:
#if IRVE_INTERNAL_CONFIG_RUST
            irvelog(indent, "pretty = %s", this->disassemble().c_str());
#endif
            irvelog(indent, "type   = S");
            irvelog(indent, "opcode = 0x%X", this->get_opcode());
            irvelog(indent, "funct3 = 0x%X", this->get_funct3());
            irvelog(indent, "rs1    = x%u", this->get_rs1());
            irvelog(indent, "rs2    = x%u", this->get_rs2());
            irvelog(indent, "imm    = 0x%X", this->get_imm());
            break;
        case InstFormat::B_TYPE:
#if IRVE_INTERNAL_CONFIG_RUST
            irvelog(indent, "pretty = %s", this->disassemble().c_str());
#endif
            irvelog(indent, "type   = B");
            irvelog(indent, "opcode = 0x%X", this->get_opcode());
            irvelog(indent, "funct3 = 0x%X", this->get_funct3());
            irvelog(indent, "rs1    = x%u", this->get_rs1());
            irvelog(indent, "rs2    = x%u", this->get_rs2());
            irvelog(indent, "imm    = 0x%X", this->get_imm());
            break;
        case InstFormat::U_TYPE:
#if IRVE_INTERNAL_CONFIG_RUST
            irvelog(indent, "pretty = %s", this->disassemble().c_str());
#endif
            irvelog(indent, "type   = U");
            irvelog(indent, "opcode = 0x%X", this->get_opcode());
            irvelog(indent, "rd     = x%u", this->get_rd());
            irvelog(indent, "imm    = 0x%X", this->get_imm());
            break;
        case InstFormat::J_TYPE:
#if IRVE_INTERNAL_CONFIG_RUST
            irvelog(indent, "pretty = %s", this->disassemble().c_str());
#endif
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

decode::InstFormat decode::DecodedInst::get_format() const {
    return this->m_format;
}

decode::Opcode decode::DecodedInst::get_opcode() const {
    return this->m_opcode;
}

//FIXME all these assertions cause problems for the SYSTEM instructions

uint8_t decode::DecodedInst::get_funct3() const {
    assert((this->get_format() != InstFormat::U_TYPE) &&
            "Attempt to get funct3 of U-type instruction!");
    assert((this->get_format() != InstFormat::J_TYPE) &&
            "Attempt to get funct3 of J-type instruction!");
    return this->m_funct3;
}

uint8_t decode::DecodedInst::get_funct5() const {
    assert((this->get_opcode() == Opcode::AMO) &&
            "Attempt to get funct5 of non-AMO instruction!");
    return this->m_funct5;
}

uint8_t decode::DecodedInst::get_funct7() const {
    //FIXME this assertion causes problems for the SYSTEM instructions (need to add an exception for them)
    //assert((this->get_format() == InstFormat::R_TYPE) && "Attempt to get funct7 of non-R-type instruction!");
    return this->m_funct7;
}

uint8_t decode::DecodedInst::get_rd() const {
    assert((this->get_format() != InstFormat::S_TYPE) &&
            "Attempt to get rd of S-type instruction!");
    assert((this->get_format() != InstFormat::B_TYPE) &&
            "Attempt to get rd of B-type instruction!");
    return this->m_rd;
}

uint8_t decode::DecodedInst::get_rs1() const {
    assert((this->get_format() != InstFormat::U_TYPE) &&
            "Attempt to get rs1 of U-type instruction!");
    assert((this->get_format() != InstFormat::J_TYPE) &&
            "Attempt to get rs1 of J-type instruction!");
    return this->m_rs1;
}

uint8_t decode::DecodedInst::get_rs2() const {
    //FIXME these assertions cause problems for the SYSTEM instructions (need to add an exception for them)
    //assert((this->get_format() != InstFormat::I_TYPE) && "Attempt to get rs2 of I-type instruction!");
    //assert((this->get_format() != InstFormat::U_TYPE) && "Attempt to get rs2 of U-type instruction!");
    //assert((this->get_format() != InstFormat::J_TYPE) && "Attempt to get rs2 of J-type instruction!");
    return this->m_rs2;
}

Word decode::DecodedInst::get_imm() const {
    switch (this->get_format()) {
        case InstFormat::R_TYPE:
            assert(false && "Attempt to get imm of R-type instruction!");
            break;
        case InstFormat::I_TYPE:
            return this->m_imm_I;
            break;
        case InstFormat::S_TYPE:
            return this->m_imm_S;
            break;
        case InstFormat::B_TYPE:
            return this->m_imm_B;
            break;
        case InstFormat::U_TYPE:
            return this->m_imm_U;
            break;
        case InstFormat::J_TYPE:
            return this->m_imm_J;
            break;
    }

    assert(false && "We should never get here");
    std::unreachable();
}

std::string decode::DecodedInst::disassemble() const {
#if IRVE_INTERNAL_CONFIG_RUST
    disassemble::DecodedInst rust_decoded_inst = {
        .format     = (disassemble::Format)this->get_format(),//NOTE: Relies on the enum numbering being the same
        .opcode     = (disassemble::Opcode)this->get_opcode(),//NOTE: Relies on the enum numbering being the same
        .rd         = this->m_rd,
        .rs1        = this->m_rs1,
        .rs2        = this->m_rs2,
        .funct3     = this->m_funct3,
        .funct5     = this->m_funct5,
        .funct7     = this->m_funct7,
        .imm        = (this->get_format() == InstFormat::R_TYPE) ? 0 : this->get_imm().u,
    };
    const char* disassembly = disassemble::disassemble(&rust_decoded_inst);
    std::string disassembly_copy = disassembly;
    disassemble::free_disassembly(disassembly);
    return disassembly_copy;
#else
    return "Disassembly not available in non-Rust IRVE builds";
#endif
}
