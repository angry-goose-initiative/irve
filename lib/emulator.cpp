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
#include "gdbserver.h"
#include "execute.h"
#include "memory.h"
#include "rvexception.h"

#define INST_COUNT this->m_CSR.implicit_read(CSR::address::MINSTRET).u
#include "logging.h"

using namespace irve::internal;

/* Function Implementations */

emulator::emulator_t::emulator_t(int imagec, const char* const* imagev):
        m_CSR(),
        m_memory(imagec, imagev, m_CSR),
        m_cpu_state(m_CSR),
        m_intercept_breakpoints(false) {
    irvelog(0, "Created new emulator instance");
}

bool emulator::emulator_t::tick() {
    this->m_CSR.implicit_write(CSR::address::MINSTRET, this->m_CSR.implicit_read(CSR::address::MINSTRET) + 1);
    this->m_CSR.implicit_write(CSR::address::MCYCLE,   this->m_CSR.implicit_read(CSR::address::MCYCLE  ) + 1);
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

    this->m_CSR.update_timer();

    //TODO also add a function to memory to update peripherals

    this->check_and_handle_interrupts();

    irvelog(0, "Tick %lu ends", this->get_inst_count());
    return true;
}

void emulator::emulator_t::run_until(uint64_t inst_count) {
    if (inst_count) {
        //Run until the given instruction count is reached or an exit request is made
        while ((this->get_inst_count() < inst_count) && this->tick());
    }
    else {
        //The only exit criteria is an exit request
        while (this->tick());
    }
}

void emulator::emulator_t::run_gdbserver(uint16_t port) {
    this->m_intercept_breakpoints = true;
    this->m_encountered_breakpoint = false;
    gdbserver::start(*this, this->m_cpu_state, this->m_memory, port);
}

uint64_t emulator::emulator_t::get_inst_count() const {
    return INST_COUNT;
}

bool emulator::emulator_t::test_and_clear_breakpoint_encountered_flag() {
    bool breakpoint_encountered = this->m_encountered_breakpoint;
    this->m_encountered_breakpoint = false;
    return breakpoint_encountered;
}

word_t emulator::emulator_t::fetch() /* const */ {//FIXME figure out why this can't be const
    irvelog(1, "Fetching from 0x%08x", this->m_cpu_state.get_pc());

    //Read a word from memory at the PC
    //NOTE: It may throw an exception for various reasons
    word_t inst = this->m_memory.instruction(this->m_cpu_state.get_pc());

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

void emulator::emulator_t::check_and_handle_interrupts() {
    //FIXME this may be wrong, read 3.1.6.1 to figure out what the hell is going on
    //Also 4.1.3
    
    /*
    reg_t mstatus = this->m_CSR.implicit_read(CSR::address::MSTATUS);
    if (!mstatus.bit(3)) {//mstatus.MIE is not set, AKA interrupts are disabled globally for all privilege modes
        return;
    }
    //If we reach this point, interrupts are enabled at least at the M-mode level

    //Get mip and sie. NOTE: We don't need to read sip and sie, since those are just shadows for M-mode code to use
    reg_t mip = this->m_CSR.implicit_read(CSR::address::MIP);
    reg_t mie = this->m_CSR.implicit_read(CSR::address::MIE);

    //Also mideleg will be useful
    reg_t mideleg = this->m_CSR.implicit_read(CSR::address::MIDELEG);

    //Check if any interrupts are pending, and choose the one with the highest priority
    //According to the spec, the order of priority is: MEI, MSI, MTI, SEI, SSI, STI
    rvexception::cause_t cause;
    //bool m_mode_interrupt;
    bool delegated_to_smode;
    if ((mip.bit(11) == 1) && (mie.bit(11) == 1)) {//MEI
        //m_mode_interrupt = true;
        cause = rvexception::cause_t::MACHINE_EXTERNAL_INTERRUPT;
        delegated_to_smode = mideleg.bit(11) == 1;
    } else if ((mip.bit(3) == 1) && (mie.bit(3) == 1)) {//MSI
        //m_mode_interrupt = true;
        cause = rvexception::cause_t::MACHINE_SOFTWARE_INTERRUPT;
        delegated_to_smode = mideleg.bit(3) == 1;
    } else if ((mip.bit(7) == 1) && (mie.bit(7) == 1)) {//MTI
        //m_mode_interrupt = true;
        cause = rvexception::cause_t::MACHINE_TIMER_INTERRUPT;
        delegated_to_smode = mideleg.bit(7) == 1;
    } else if ((mip.bit(9) == 1) && (mie.bit(9) == 1)) {//SEI
        //m_mode_interrupt = false;
        cause = rvexception::cause_t::SUPERVISOR_EXTERNAL_INTERRUPT;
        delegated_to_smode = mideleg.bit(9) == 1;
    } else if ((mip.bit(1) == 1) && (mie.bit(1) == 1)) {//SSI
        //m_mode_interrupt = false;
        cause = rvexception::cause_t::SUPERVISOR_SOFTWARE_INTERRUPT;
        delegated_to_smode = mideleg.bit(1) == 1;
    } else if ((mip.bit(5) == 1) && (mie.bit(5) == 1)) {//STI
        //m_mode_interrupt = false;
        cause = rvexception::cause_t::SUPERVISOR_TIMER_INTERRUPT;
        delegated_to_smode = midleg.bit(5) == 1;
    } else {//No interrupts pending
        return;
    }
    //If we made it here, at least one interrupt is pending
    //Also the details about the interrupt of highest priority are known

    //bool interrupt_while_in_machine_mode = this->m_CSR.get_privilege_mode() == CSR::privilege_mode_t::MACHINE_MODE;
    assert(false && "TODO interrupts not yet handled");
    */

    /*
    bool supervisor_software_interrupt_pending = mip.bit(1) == 1;
    bool machine_software_interrupt_pending = mip.bit(3) == 1;
    bool supervisor_timer_interrupt_pending = mip.bit(5) == 1;
    bool machine_timer_interrupt_pending = mip.bit(7) == 1;
    bool supervisor_external_interrupt_pending = mip.bit(9) == 1;
    bool machine_external_interrupt_pending = mip.bit(11) == 1;


    bool interrupt_while_in_machine_mode = this->m_CSR.get_privilege_mode() == CSR::privilege_mode_t::MACHINE_MODE;
    */
    //if interrupt occured
    //this->m_cpu_state.invalidate_reservation_set();//Could have interrupted an LR/SC sequence
    //assert(false && "TODO interrupts not yet handled");//TODO handle interrupts
}

void emulator::emulator_t::handle_exception(rvexception::cause_t cause) {
    this->m_cpu_state.invalidate_reservation_set();//Could have interrupted an LR/SC sequence
    
    //TODO better logging

    uint32_t raw_cause = (uint32_t)cause;
    assert((raw_cause < 32) && "Unsuppored cause value!");//Makes it simpler since this means we must check medeleg always
    irvelog(1, "Handling exception: Cause: %u", raw_cause);

    if (this->m_intercept_breakpoints && (cause == rvexception::cause_t::BREAKPOINT_EXCEPTION) && (this->m_CSR.get_privilege_mode() != CSR::privilege_mode_t::USER_MODE)) {
        irvelog(1, "Breakpoint intercepted");
        this->m_encountered_breakpoint = true;
        return;
    }

    bool exception_from_machine_mode = this->m_CSR.get_privilege_mode() == CSR::privilege_mode_t::MACHINE_MODE;
    bool exception_delegated_to_machine_mode = this->m_CSR.implicit_read(CSR::address::MEDELEG).bit(raw_cause) == 0;

    if (exception_from_machine_mode || exception_delegated_to_machine_mode) {//Exception should be handled in machine mode
        //Manage the privilege stack
        word_t mstatus = this->m_CSR.implicit_read(CSR::address::MSTATUS);
        word_t mie = mstatus.bit(3);
        mstatus &= 0b11111111111111111110011101110111;//Clear the MPP, MPIE, and MIE bits
        mstatus |= ((uint32_t)this->m_CSR.get_privilege_mode()) << 11;//Set the MPP bits to the current privilege mode
        mstatus |= mie << 7;//Set MPIE to MIE
        //MIE is set to 0
        this->m_CSR.implicit_write(CSR::address::MSTATUS, mstatus);//Write changes back to the CSR
        this->m_CSR.set_privilege_mode(CSR::privilege_mode_t::MACHINE_MODE);

        //Write other CSRs to indicate information about the exception
        this->m_CSR.implicit_write(CSR::address::MCAUSE, (uint32_t) cause);
        this->m_CSR.implicit_write(CSR::address::MEPC, this->m_cpu_state.get_pc());
        this->m_CSR.implicit_write(CSR::address::MTVAL, 0);

        //Jump to the exception handler
        this->m_cpu_state.set_pc(this->m_CSR.implicit_read(CSR::address::MTVEC).bits(31, 2));
    } else {//Exception should be handled by supervisor mode
        //Manage the privilege stack
        word_t sstatus = this->m_CSR.implicit_read(CSR::address::SSTATUS);
        word_t sie = sstatus.bit(1);
        sstatus &= 0b11111111111111111111111011011101;//Clear the SPP, SPIE, and SIE bits
        sstatus |= ((this->m_CSR.get_privilege_mode() == CSR::privilege_mode_t::SUPERVISOR_MODE) ? 0b1 : 0b0) << 8;//Set the SPP bit appropriately
        sstatus |= sie << 5;//Set SPIE to SIE
        //SIE is set to 0
        this->m_CSR.implicit_write(CSR::address::SSTATUS, sstatus);//Write changes back to the CSR
        this->m_CSR.set_privilege_mode(CSR::privilege_mode_t::SUPERVISOR_MODE);

        //Write other CSRs to indicate information about the exception
        this->m_CSR.implicit_write(CSR::address::SCAUSE, (uint32_t) cause);
        this->m_CSR.implicit_write(CSR::address::SEPC, this->m_cpu_state.get_pc());
        this->m_CSR.implicit_write(CSR::address::STVAL, 0);

        //Jump to the exception handler
        this->m_cpu_state.set_pc(this->m_CSR.implicit_read(CSR::address::STVEC).bits(31, 2));
    }
}
