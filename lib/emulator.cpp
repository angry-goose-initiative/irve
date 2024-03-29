/**
 * @brief   The main emulator class used to instantiate an instance of IRVE.
 * 
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  Copyright (C) 2023 Nick Chan\n
 *  See the LICENSE file at the root of the project for licensing info.
*/

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include "emulator.h"

#include <cassert>
#include <cstdint>

#include "common.h"
#include "cpu_state.h"
#include "csr.h"
#include "decode.h"
#include "gdbserver.h"
#include "execute.h"
#include "memory.h"
#include "rv_trap.h"
#include "semihosting.h"

#define INST_COUNT this->m_CSR.implicit_read(Csr::Address::MINSTRET).u
#include "logging.h"

using namespace irve::internal;

/* ------------------------------------------------------------------------------------------------
 * Constants
 * --------------------------------------------------------------------------------------------- */

constexpr uint32_t MAX_PERIPHERAL_UPDATE_DELAY_COUNTER_VALUE = 65535;

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

emulator::emulator_t::emulator_t(int imagec, const char* const* imagev):
    m_CSR(),
    m_memory(imagec, imagev, m_CSR),
    m_cpu_state(),
    m_intercept_breakpoints(false),
    m_peripheral_update_delay_counter(MAX_PERIPHERAL_UPDATE_DELAY_COUNTER_VALUE)
{
    irvelog(0, "Created new emulator instance");
}

bool emulator::emulator_t::tick() {
    this->m_CSR.increment_perf_counters();
    irvelog(0, "Tick %lu begins", this->get_inst_count());

    //Any of these could lead to exceptions (ex. faults, illegal instructions, etc.)
    try {
        decode::DecodedInst decoded_inst = this->fetch_and_decode();
        this->execute(decoded_inst);
    } catch (const rv_trap::RvException& e) {
        assert(((uint32_t)e.cause() < 32) && "Unsuppored cause value!");
        irvelog(1, "Handling exception: Cause: %u", (uint32_t)e.cause());
        this->handle_trap(e.cause(), e.tval());
    } catch (const rv_trap::IrveExitRequest&) {
        irvelog(0, "Recieved exit request from emulated guest");
        return false;
    }

    //Only actually update the timer and peripherals every once in a while, rather than each time
    //this function is called. This is since chrono (used by the timer) and the read syscall
    //(used by the UART) are REALLY REALLY REALLY slow.
    --this->m_peripheral_update_delay_counter;
    if (this->m_peripheral_update_delay_counter == 0) {
        //Reset the delay counter
        this->m_peripheral_update_delay_counter = MAX_PERIPHERAL_UPDATE_DELAY_COUNTER_VALUE;

        //May or may not set the timer interrupt pending bit depending on if the timer has expired
        this->m_CSR.update_timer();

        //Update peripherals and potentially set the external interrupt pending bit
        this->m_memory.update_peripherals();
    }

    //May need to deal with interrupt if they were set by one of the above functions,
    //or if a software interrupt pending bit was set by the instruction executed
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

uint64_t emulator::emulator_t::get_inst_count() {
    return INST_COUNT;
}

bool emulator::emulator_t::test_and_clear_breakpoint_encountered_flag() {
    bool breakpoint_encountered = this->m_encountered_breakpoint;
    this->m_encountered_breakpoint = false;
    return breakpoint_encountered;
}

void emulator::emulator_t::flush_icache() {
    this->m_icache.clear();
}

decode::DecodedInst emulator::emulator_t::fetch_and_decode() {
    Word pc = this->m_cpu_state.get_pc();
    irvelog(1, "Fetching from 0x%08x", pc);

    //Note: Using exceptions instead to catch misses is (very slightly) faster when using the same
    //      few instructions over and over again. (ex in nouveau_stress_test). But it tanks
    //      performance in other scenarios so we do compare-and-branch instead.
    if (this->m_icache.contains(pc.u)) {
        irvelog(1, "Cache hit");
        return this->m_icache.at(pc.u);
    } else {
        irvelog(1, "Cache miss");

        //Read a word from memory at the PC
        //NOTE: It may throw an exception for various reasons
        Word inst = this->m_memory.instruction(pc);

        //Log what we fetched and return it
        irvelog(1, "Fetched 0x%08x from 0x%08x", inst, pc);

        irvelog(1, "Decoding instruction 0x%08X", inst);
        decode::DecodedInst decoded_inst(inst);
        decoded_inst.log(2, this->get_inst_count());

        //TODO be more fine-grained about this
        //Note the icache is cleared on exceptions and interrupts, so that is already handled
        if (decoded_inst.get_opcode() == decode::Opcode::MISC_MEM) {//To catch FENCE.i
            this->flush_icache();
        } else if (decoded_inst.get_opcode() == decode::Opcode::SYSTEM) {//To catch satp changes, SFENCE.VMA
            this->flush_icache();
        } else {//There is no need to clear the cache
            this->m_icache.emplace(pc.u, decoded_inst);
        }

        return decoded_inst;
    }
}

//TODO move this to a separate file maybe?
void emulator::emulator_t::execute(const decode::DecodedInst &decoded_inst) {
    irvelog(1, "Executing instruction");

    //We can assume the opcode exists since the instruction is valid
    switch (decoded_inst.get_opcode()) {
        case decode::Opcode::LOAD:
            assert((decoded_inst.get_format() == decode::InstFormat::I_TYPE) && "Instruction with LOAD opcode had a non-I format!");
            execute::load(decoded_inst, this->m_cpu_state, this->m_memory, this->m_CSR);
            break;
        case decode::Opcode::CUSTOM_0:
            assert((decoded_inst.get_format() == decode::InstFormat::R_TYPE) && "Instruction with CUSTOM_0 opcode had a non-R format!");
            execute::custom_0(decoded_inst, this->m_cpu_state, this->m_memory, this->m_CSR);
            break;
        case decode::Opcode::MISC_MEM:
            assert((decoded_inst.get_format() == decode::InstFormat::I_TYPE) && "Instruction with MISC_MEM opcode had a non-I format!");
            execute::misc_mem(decoded_inst, this->m_cpu_state, this->m_CSR);
            break;
        case decode::Opcode::OP_IMM:
            assert((decoded_inst.get_format() == decode::InstFormat::I_TYPE) && "Instruction with OP_IMM opcode had a non-I format!");
            execute::op_imm(decoded_inst, this->m_cpu_state, this->m_CSR);
            break;
        case decode::Opcode::AUIPC:
            assert((decoded_inst.get_format() == decode::InstFormat::U_TYPE) && "Instruction with AUIPC opcode had a non-U format!");
            execute::auipc(decoded_inst, this->m_cpu_state, this->m_CSR);
            break;
        case decode::Opcode::STORE:
            assert((decoded_inst.get_format() == decode::InstFormat::S_TYPE) && "Instruction with STORE opcode had a non-S format!");
            execute::store(decoded_inst, this->m_cpu_state, this->m_memory, this->m_CSR);
            break;
        case decode::Opcode::AMO:
            //TODO assertion
            execute::amo(decoded_inst, this->m_cpu_state, this->m_memory, this->m_CSR);
            break;
        case decode::Opcode::OP:
            assert((decoded_inst.get_format() == decode::InstFormat::R_TYPE) && "Instruction with OP opcode had a non-R format!");
            execute::op(decoded_inst, this->m_cpu_state, this->m_CSR);
            break;
        case decode::Opcode::LUI:
            assert((decoded_inst.get_format() == decode::InstFormat::U_TYPE) && "Instruction with LUI opcode had a non-U format!");
            execute::lui(decoded_inst, this->m_cpu_state, this->m_CSR);
            break;
        case decode::Opcode::BRANCH:
            assert((decoded_inst.get_format() == decode::InstFormat::B_TYPE) && "Instruction with BRANCH opcode had a non-B format!");
            execute::branch(decoded_inst, this->m_cpu_state, this->m_CSR);
            break;
        case decode::Opcode::JALR:
            assert((decoded_inst.get_format() == decode::InstFormat::I_TYPE) && "Instruction with JALR opcode had a non-I format!");
            execute::jalr(decoded_inst, this->m_cpu_state, this->m_CSR);
            break;
        case decode::Opcode::JAL:
            assert((decoded_inst.get_format() == decode::InstFormat::J_TYPE) && "Instruction with JAL opcode had a non-J format!");
            execute::jal(decoded_inst, this->m_cpu_state, this->m_CSR);
            break;
        case decode::Opcode::SYSTEM:
            assert((decoded_inst.get_format() == decode::InstFormat::I_TYPE) && "Instruction with SYSTEM opcode had a non-I format!");
            execute::system(decoded_inst, this->m_cpu_state, this->m_CSR);
            break;
        default:
            assert(false && "Instruction with either invalid opcode, or that is implemented in decode but not in execute yet!");
            break;
    }
}

void emulator::emulator_t::check_and_handle_interrupts() {
    //Section 3.1.6.1 describes when interrupts are globally enabled
    //We will never be interrupted to a lower privilege level if we are currently executing in a higher privilege level
    //Ex. if we're in M-mode, interrupts delegated to S-mode will never be triggered (will just stay pending)
    //However it does work the other way around: if we're in S-mode or U-mode, interrupts delegated to M-mode will be triggered
    //
    //Now let's consider MIE and SIE.
    //Critically these ONLY TAKE AFFECT IF WE'RE IN THE CORRESPONDING PRIVILEGE MODE
    //So if we're in M-mode and MIE is clear, we will never be interrupted
    //But if we're in S-mode and MIE is clear, we will still be interrupted to M-mode
    //This is similar for SIE: if we're in S-mode and SIE is clear, we will never be interrupted, but if we're in U-mode and SIE is clear, we will still be interrupted to S-mode
    //
    //Finally, an interupts only occurs if it is pending (mip/sip) and enabled (mie/sie)
    //
    //Note: 4.1.3 may also be useful

    irvelog(1, "Checking for interrupts...");

    auto interrupt_regs = this->m_CSR.fast_implicit_read_interrupt_regs();

    Reg mstatus = interrupt_regs.mstatus;
    bool in_m_mode = interrupt_regs.privilege_mode == PrivilegeMode::MACHINE_MODE;
    bool in_s_mode = interrupt_regs.privilege_mode == PrivilegeMode::SUPERVISOR_MODE;

    bool global_enable_for_current_privilege = (in_m_mode && (mstatus.bit(3) == 1)) || (in_s_mode && (mstatus.bit(1) == 1));

    //Helper lambda. Returns true if the given bit is "interrupting". Aka, that...
    //1. The interrupt is pending (mip/sip)
    //2. The interrupt is enabled (mie/sie)
    //3. Interrupts are globally enabled at this level, or the interrupt is for a higher privilege level (mstatus.MIE/SIE)
    //4. The interrupt is delegated to the current or a higher privilege level (mideleg/sideleg)
    //Also sets `delegated_to_smode` properly for later use
    auto is_interrupting = [&](uint8_t bit) {
        assert((bit < 32) && "Invalid is_interrupting() bit!");
        //NOTE: We don't need to read sip and sie, since those are just shadows for S-mode code to use
        bool pending            = interrupt_regs.mip.bit(bit)       == 1;
        bool enabled            = interrupt_regs.mie.bit(bit)       == 1;
        bool delegated_to_smode = interrupt_regs.mideleg.bit(bit)   == 1;

        //Either in user mode, or in s-mode but it is delegated to m-mode
        bool interrupt_is_for_higher_privilege_level = !in_m_mode && (!in_s_mode || (in_s_mode && !delegated_to_smode));

        //Effective global enable based on mstatus bits, or always if the interrupt is for a higher privilege level
        bool effective_global_enable = interrupt_is_for_higher_privilege_level || global_enable_for_current_privilege;

        bool interrupt_is_for_lower_privilege_level = in_m_mode && delegated_to_smode;

        bool interrupting = pending && enabled && effective_global_enable && !interrupt_is_for_lower_privilege_level;
        return interrupting;
    };

    //According to the spec, the order of priority is: MEI, MSI, MTI, SEI, SSI, STI
    rv_trap::Cause cause;
    if        (is_interrupting(11)) {//MEI
        cause = rv_trap::Cause::MACHINE_EXTERNAL_INTERRUPT;
    } else if (is_interrupting( 3)) {//MSI
        cause = rv_trap::Cause::MACHINE_SOFTWARE_INTERRUPT;
    } else if (is_interrupting( 7)) {//MTI
        cause = rv_trap::Cause::MACHINE_TIMER_INTERRUPT;
    } else if (is_interrupting( 9)) {//SEI
        cause = rv_trap::Cause::SUPERVISOR_EXTERNAL_INTERRUPT;
    } else if (is_interrupting( 1)) {//SSI
        cause = rv_trap::Cause::SUPERVISOR_SOFTWARE_INTERRUPT;
    } else if (is_interrupting( 5)) {//STI
        cause = rv_trap::Cause::SUPERVISOR_TIMER_INTERRUPT;
    } else {
        irvelog(1, "No interrupts \"interrupting\" at this time.");
        return;
    }
    //If we make it here, we have an interrupt to handle (specifically the one in `cause)

    irvelog(1, "Handling interrupt: Cause: 0x%X", (uint32_t)cause);
    this->handle_trap(cause, 0);
}

void emulator::emulator_t::handle_trap(rv_trap::Cause cause, Word tval) {
    this->flush_icache();

    //TODO better logging

    //There is some special handling for the breakpoint exception
    if (cause == rv_trap::Cause::BREAKPOINT_EXCEPTION) {
        //Check if this is a semihosting call
        //We ONLY handle semihosting calls in M-mode, since the firmware should take care of ones from S-mode
        //This is okay to do in M-mode because, with a real hardware debugger, it would be intercepting things in this way too
        //(either in hardware or, for example, OpenOCD on the host would be reading behind and ahead to detect the semihosting call)
        bool semihosting_ebreak = false;
        if (this->m_CSR.get_privilege_mode() == PrivilegeMode::MACHINE_MODE) {
            //Try to access the previous and next instructions in memory surrounding the EBREAK
            //It helps that we're guaranteed the instructions are always uncompressed (4 bytes)
            try {
                Reg pc = this->m_cpu_state.get_pc();
                Word prev_inst = this->m_memory.instruction(pc - 4);
                Word next_inst = this->m_memory.instruction(pc + 4);

                //Detect the semihosting sequence:
                //slli x0, x0, 0x1F
                //ebreak
                //srai x0, x0, 0x7
                if ((prev_inst == 0x01f01013) && (next_inst == 0x40705013)) {
                    semihosting_ebreak = true;
                }
                //Otherwise not a semihosting ebreak
            } catch (const rv_trap::RvException&) {
                //Not a semihosting ebreak
            }
        }

        bool interscept_ebreak = this->m_intercept_breakpoints && (this->m_CSR.get_privilege_mode() != PrivilegeMode::USER_MODE);

        //Semihosting takes priority over breakpoint interception (hopefully this dosn't cause too many headaches)
        if (semihosting_ebreak) {
            irvelog(1, "Semihosting EBREAK detected");
            this->m_semihosting_handler.handle(this->m_cpu_state, this->m_memory);
            return;
        } else if (interscept_ebreak) {
            //We do this to support debugging with irvegdb. This will cause issues if anything other than GDB puts an EBREAK instruction in memory
            //Really though we should only do this for M-mode, and the firmware should simply forward EBREAKs from S-mode to M-mode somehow
            //But this does work fine enough as-is
            //We only intercept non-semihosting EBREAKs if we're in M-mode
            irvelog(1, "Breakpoint intercepted");
            this->m_encountered_breakpoint = true;
            return;
        }
        //Otherwise, let RISC-V code handle the EBREAK
    }

    Word raw_cause = (uint32_t)cause;
    assert((raw_cause.bits(30, 0).u < 32) && "Unsupported cause!");

    bool is_interrupt = raw_cause.bit(31) == 1;

    bool handle_in_m_mode;
    if (is_interrupt) {//The cause was an interrupt
        //With interrupts, they don't "come" from a particular mode
        //We assume check_and_handle_interrupts() has already checked that the interrupt is "interrupting", so this should be enough
        bool interrupt_delegated_to_machine_mode = this->m_CSR.implicit_read(Csr::Address::MIDELEG).bit(raw_cause.bits(30, 0)) == 0;
        handle_in_m_mode = interrupt_delegated_to_machine_mode;
    } else {//The cause was an exception
        bool exception_from_machine_mode = this->m_CSR.get_privilege_mode() == PrivilegeMode::MACHINE_MODE;
        bool exception_delegated_to_machine_mode = this->m_CSR.implicit_read(Csr::Address::MEDELEG).bit(raw_cause) == 0;
        handle_in_m_mode = exception_from_machine_mode || exception_delegated_to_machine_mode;
    }

    if (handle_in_m_mode) {//Exception should be handled in machine mode
        //Manage the privilege stack
        Word mstatus = this->m_CSR.implicit_read(Csr::Address::MSTATUS);
        Word mie = mstatus.bit(3);
        mstatus &= 0b11111111111111111110011101110111;//Clear the MPP, MPIE, and MIE bits
        mstatus |= ((uint32_t)this->m_CSR.get_privilege_mode()) << 11;//Set the MPP bits to the current privilege mode
        mstatus |= mie << 7;//Set MPIE to MIE
        //MIE is set to 0
        this->m_CSR.implicit_write(Csr::Address::MSTATUS, mstatus);//Write changes back to the CSR
        this->m_CSR.set_privilege_mode(PrivilegeMode::MACHINE_MODE);

        //Write other CSRs to indicate information about the exception
        this->m_CSR.implicit_write(Csr::Address::MCAUSE, (uint32_t) cause);
        this->m_CSR.implicit_write(Csr::Address::MEPC, this->m_cpu_state.get_pc());
        this->m_CSR.implicit_write(Csr::Address::MTVAL, tval);

        //Jump to the exception handler
        Word mtvec = this->m_CSR.implicit_read(Csr::Address::MTVEC);
        bool vectored = mtvec.bits(1, 0) == 0b01;
        Word vector_table_base_addr = mtvec & 0xFFFFFFFC;
        if (vectored && is_interrupt) {
            this->m_cpu_state.set_pc(vector_table_base_addr + (raw_cause.bits(30, 0) * 4));
        } else {
            this->m_cpu_state.set_pc(vector_table_base_addr);
        }
    } else {//Exception should be handled by supervisor mode
        //Manage the privilege stack
        Word sstatus = this->m_CSR.implicit_read(Csr::Address::SSTATUS);
        Word sie = sstatus.bit(1);
        sstatus &= 0b11111111111111111111111011011101;//Clear the SPP, SPIE, and SIE bits
        sstatus |= ((this->m_CSR.get_privilege_mode() == PrivilegeMode::SUPERVISOR_MODE) ? 0b1 : 0b0) << 8;//Set the SPP bit appropriately
        sstatus |= sie << 5;//Set SPIE to SIE
        //SIE is set to 0
        this->m_CSR.implicit_write(Csr::Address::SSTATUS, sstatus);//Write changes back to the CSR
        this->m_CSR.set_privilege_mode(PrivilegeMode::SUPERVISOR_MODE);

        //Write other CSRs to indicate information about the exception
        this->m_CSR.implicit_write(Csr::Address::SCAUSE, (uint32_t) cause);
        this->m_CSR.implicit_write(Csr::Address::SEPC, this->m_cpu_state.get_pc());
        this->m_CSR.implicit_write(Csr::Address::STVAL, tval);

        //Jump to the exception handler
        Word stvec = this->m_CSR.implicit_read(Csr::Address::STVEC);
        bool vectored = stvec.bits(1, 0) == 0b01;
        Word vector_table_base_addr = stvec & 0xFFFFFFFC;
        if (vectored && is_interrupt) {
            this->m_cpu_state.set_pc(vector_table_base_addr + (raw_cause.bits(30, 0) * 4));
        } else {
            this->m_cpu_state.set_pc(vector_table_base_addr);
        }
    }
}
