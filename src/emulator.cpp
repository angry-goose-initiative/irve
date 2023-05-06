/* emulator.cpp
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * The main emulator class used to instantiate an instance of irve
 *
*/

/* Includes */

#include "emulator.h"

#include <cassert>
#include <cstdint>

#include "common.h"
#include "decode.h"
#include "execute.h"

#define INST_COUNT this->get_inst_count()
#include "logging.h"

/* Function Implementations */

emulator_t::emulator_t() : m_memory(), m_cpu_state(m_memory) {
    irvelog(0, "Created new emulator instance");
}

void emulator_t::tick() {
    this->m_cpu_state.increment_inst_count();
    irvelog(0, "Tick %lu begins", this->get_inst_count());

    word_t inst = this->fetch();

    irvelog(1, "Decoding instruction 0x%08X", inst);
    decoded_inst_t decoded_inst(inst);
    decoded_inst.log(2, this->get_inst_count());

    this->execute(decoded_inst);

    irvelog(1, "TODO handle interrupts");

    irvelog(0, "Tick %lu ends", this->get_inst_count());
}

uint64_t emulator_t::get_inst_count() const {
    return this->m_cpu_state.get_inst_count();
}

int8_t emulator_t::mem_read_byte(uint32_t addr) const {
    return (int8_t)this->m_memory.r(addr, 0b000);
}

void emulator_t::mem_write(uint32_t addr, uint8_t size, int32_t data) {
    this->m_memory.w(addr, size, data);
}

word_t emulator_t::fetch() const {
    //Read a word from memory at the PC (using a "funct3" of 0b010 to get 32 bits)
    word_t inst = this->m_memory.r(this->m_cpu_state.get_pc(), 0b010);

    //Log what we fetched and return it
    irvelog(1, "Fetched 0x%08x from 0x%08x", inst, this->m_cpu_state.get_pc());
    return inst;
}

//TODO move this to a separate file
void emulator_t::execute(const decoded_inst_t &decoded_inst) {
    irvelog(1, "Executing instruction");

    assert(decoded_inst.is_valid() && "TODO handle invalid instructions");

    //We can assume the opcode exists since the instruction is valid
    switch (decoded_inst.get_opcode()) {
        case LOAD:
            assert((decoded_inst.get_format() == I_TYPE) && "Instruction with LOAD opcode had a non-I format!");
            execute::load(decoded_inst, this->m_cpu_state, this->m_memory);
            break;
        case MISC_MEM:
            assert((decoded_inst.get_format() == I_TYPE) && "Instruction with MISC_MEM opcode had a non-I format!");
            execute::misc_mem(decoded_inst, this->m_cpu_state);
            break;
        case OP_IMM:
            assert((decoded_inst.get_format() == I_TYPE) && "Instruction with OP_IMM opcode had a non-I format!");
            execute::op_imm(decoded_inst, this->m_cpu_state);
            break;
        case AUIPC:
            assert((decoded_inst.get_format() == U_TYPE) && "Instruction with AUIPC opcode had a non-U format!");
            execute::auipc(decoded_inst, this->m_cpu_state);
            break;
        case STORE:
            assert((decoded_inst.get_format() == S_TYPE) && "Instruction with STORE opcode had a non-S format!");
            execute::store(decoded_inst, this->m_cpu_state, this->m_memory);
            break;
        case AMO:
            //TODO assertion
            execute::amo(decoded_inst, this->m_cpu_state, this->m_memory);
            break;
        case OP:
            assert((decoded_inst.get_format() == R_TYPE) && "Instruction with OP opcode had a non-R format!");
            execute::op(decoded_inst, this->m_cpu_state);
            break;
        case LUI:
            assert((decoded_inst.get_format() == U_TYPE) && "Instruction with LUI opcode had a non-U format!");
            execute::lui(decoded_inst, this->m_cpu_state);
            break;
        case BRANCH:
            assert((decoded_inst.get_format() == B_TYPE) && "Instruction with BRANCH opcode had a non-B format!");
            execute::branch(decoded_inst, this->m_cpu_state);
            break;
        case JALR:
            assert((decoded_inst.get_format() == I_TYPE) && "Instruction with JALR opcode had a non-I format!");
            execute::jalr(decoded_inst, this->m_cpu_state);
            break;
        case JAL:
            assert((decoded_inst.get_format() == J_TYPE) && "Instruction with JAL opcode had a non-J format!");
            execute::jal(decoded_inst, this->m_cpu_state);
            break;
        case SYSTEM:
            assert((decoded_inst.get_format() == I_TYPE) && "Instruction with SYSTEM opcode had a non-I format!");
            execute::system(decoded_inst, this->m_cpu_state, this->m_memory);
            break;
        default:
            assert(false && "Unimplemented opcode or not handled (maybe it should be?)");//TODO
            break;
    }
}
