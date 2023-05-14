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
#include "cpu_state.h"
#include "CSR.h"
#include "decode.h"
#include "execute.h"
#include "memory.h"
#include "rvexception.h"

#define INST_COUNT this->get_inst_count()
#include "logging.h"

using namespace irve::internal;

/* Function Implementations */

emulator::emulator_t::emulator_t() : m_memory(m_CSR), m_cpu_state(m_CSR) {
    irvelog(0, "Created new emulator instance");
}

bool emulator::emulator_t::tick() {
    this->m_CSR.increment_inst_count();//FIXME EBREAK and ECALL should not increment this apparently (see section 3.3.1 of the RISC-V spec vol 2)
    irvelog(0, "Tick %lu begins", this->get_inst_count());

    //Any of these could lead to exceptions (ex. faults, illegal instructions, etc.)
    try {
        word_t inst = this->fetch();

        irvelog(1, "Decoding instruction 0x%08X", inst);
        decode::decoded_inst_t decoded_inst(inst);
        decoded_inst.log(2, this->get_inst_count());

        this->execute(decoded_inst);
    } catch (const rvexception::rvexception_t& e) {
        this->handle_exception(e.cause());
    } catch (const rvexception::irve_exit_request_t&) {
        irvelog(0, "Recieved exit request from emulated guest");
        return false;
    }

    //TODO Each peripheral's tick() function should be called here
    //Each must be wrapped ITS OWN UNIQUE try-catch block to catch any interrupts they throw
    //while still ensuring all are ticked this major tick
    try {
        irvelog(1, "TODO tick peripherals here, and if they request an interrupt, they'll throw an exception which we'll catch");
    } catch (const rvexception::rvinterrupt_t& e) {
        this->handle_interrupt(e.cause());
    }
    //TODO One try-catch block per peripheral here...

    irvelog(0, "Tick %lu ends", this->get_inst_count());
    return true;
}

void emulator::emulator_t::run_until(uint64_t inst_count) {
    if (inst_count) {
        //Run until the given instruction count is reached or an exit request is made
        while ((this->get_inst_count() < inst_count) && this->tick());
    } else {
        //The only exit criteria is an exit request
        while (this->tick());
    }
}

uint64_t emulator::emulator_t::get_inst_count() const {
    return this->m_CSR.get_inst_count();
}

int8_t emulator::emulator_t::mem_read_byte(word_t addr) const {
    return (int8_t)this->m_memory.r(addr, 0b000).u;
}

void emulator::emulator_t::mem_write(word_t addr, uint8_t size, word_t data) {
    this->m_memory.w(addr, size, data);
}

word_t emulator::emulator_t::fetch() const {
    //Throw an exception if the PC is not aligned to a word boundary
    //TODO priority of this exception vs. others?
    if ((this->m_cpu_state.get_pc().u % 4) != 0) {
        invoke_rv_exception_with_cause(INSTRUCTION_ADDRESS_MISALIGNED_EXCEPTION);
    }

    //Read a word from memory at the PC (using a "funct3" of 0b010 to get 32 bits)
    //NOTE: It may throw an exception for various reasons
    word_t inst = this->m_memory.r(this->m_cpu_state.get_pc(), 0b010);

    //Log what we fetched and return it
    irvelog(1, "Fetched 0x%08x from 0x%08x", inst, this->m_cpu_state.get_pc());
    return inst;
}

//TODO move this to a separate file maybe?
void emulator::emulator_t::execute(const decode::decoded_inst_t &decoded_inst) {
    irvelog(1, "Executing instruction");

    //We can assume the opcode exists since the instruction is valid
    switch (decoded_inst.get_opcode()) {
        case decode::opcode_t::LOAD:
            assert((decoded_inst.get_format() == decode::inst_format_t::I_TYPE) && "Instruction with LOAD opcode had a non-I format!");
            execute::load(decoded_inst, this->m_cpu_state, this->m_memory, this->m_CSR);
            break;
        case decode::opcode_t::CUSTOM_0:
            assert((decoded_inst.get_format() == decode::inst_format_t::R_TYPE) && "Instruction with CUSTOM_0 opcode had a non-R format!");
            execute::custom_0(decoded_inst, this->m_cpu_state, this->m_memory, this->m_CSR);
            break;
        case decode::opcode_t::MISC_MEM:
            assert((decoded_inst.get_format() == decode::inst_format_t::I_TYPE) && "Instruction with MISC_MEM opcode had a non-I format!");
            execute::misc_mem(decoded_inst, this->m_cpu_state, this->m_CSR);
            break;
        case decode::opcode_t::OP_IMM:
            assert((decoded_inst.get_format() == decode::inst_format_t::I_TYPE) && "Instruction with OP_IMM opcode had a non-I format!");
            execute::op_imm(decoded_inst, this->m_cpu_state, this->m_CSR);
            break;
        case decode::opcode_t::AUIPC:
            assert((decoded_inst.get_format() == decode::inst_format_t::U_TYPE) && "Instruction with AUIPC opcode had a non-U format!");
            execute::auipc(decoded_inst, this->m_cpu_state, this->m_CSR);
            break;
        case decode::opcode_t::STORE:
            assert((decoded_inst.get_format() == decode::inst_format_t::S_TYPE) && "Instruction with STORE opcode had a non-S format!");
            execute::store(decoded_inst, this->m_cpu_state, this->m_memory, this->m_CSR);
            break;
        case decode::opcode_t::AMO:
            //TODO assertion
            execute::amo(decoded_inst, this->m_cpu_state, this->m_memory, this->m_CSR);
            break;
        case decode::opcode_t::OP:
            assert((decoded_inst.get_format() == decode::inst_format_t::R_TYPE) && "Instruction with OP opcode had a non-R format!");
            execute::op(decoded_inst, this->m_cpu_state, this->m_CSR);
            break;
        case decode::opcode_t::LUI:
            assert((decoded_inst.get_format() == decode::inst_format_t::U_TYPE) && "Instruction with LUI opcode had a non-U format!");
            execute::lui(decoded_inst, this->m_cpu_state, this->m_CSR);
            break;
        case decode::opcode_t::BRANCH:
            assert((decoded_inst.get_format() == decode::inst_format_t::B_TYPE) && "Instruction with BRANCH opcode had a non-B format!");
            execute::branch(decoded_inst, this->m_cpu_state, this->m_CSR);
            break;
        case decode::opcode_t::JALR:
            assert((decoded_inst.get_format() == decode::inst_format_t::I_TYPE) && "Instruction with JALR opcode had a non-I format!");
            execute::jalr(decoded_inst, this->m_cpu_state, this->m_CSR);
            break;
        case decode::opcode_t::JAL:
            assert((decoded_inst.get_format() == decode::inst_format_t::J_TYPE) && "Instruction with JAL opcode had a non-J format!");
            execute::jal(decoded_inst, this->m_cpu_state, this->m_CSR);
            break;
        case decode::opcode_t::SYSTEM:
            assert((decoded_inst.get_format() == decode::inst_format_t::I_TYPE) && "Instruction with SYSTEM opcode had a non-I format!");
            execute::system(decoded_inst, this->m_cpu_state, this->m_CSR);
            break;
        default:
            assert(false && "Instruction with either invalid opcode, or that is implemented in decode but not in execute yet!");
            break;
    }
}

void emulator::emulator_t::handle_interrupt(rvexception::cause_t cause) {
    this->m_cpu_state.invalidate_reservation_set();//Could have interrupted an LR/SC sequence
    assert(false && "TODO interrupts not yet handled");//TODO handle interrupts
}

void emulator::emulator_t::handle_exception(rvexception::cause_t cause) {
    this->m_cpu_state.invalidate_reservation_set();//Could have interrupted an LR/SC sequence
     
    uint32_t raw_cause = (uint32_t)cause;
    assert((raw_cause < 32) && "Unsuppored cause value!");//Makes it simpler since this means we must check medeleg always
    irvelog(1, "Handling exception: Cause: %u", raw_cause);

    //Decide which privilege mode should handle the exception (and thus which one we should switch to)
    if (this->m_CSR.medeleg[raw_cause]) {//Supervisor mode should handle the exception
        //TODO handle this case
        assert(false && "TODO handle this case");
    } else {//Machine mode should handle the exception
        //TODO manage the privilege stack in mstatus?
        this->m_CSR.set_privilege_mode(CSR::privilege_mode_t::MACHINE_MODE);

        this->m_CSR.mcause = cause;
        this->m_CSR.mepc = this->m_cpu_state.get_pc();
        this->m_cpu_state.set_pc(MTVEC.srl(2));

        //TODO what else should be done if anything?
    }
}
