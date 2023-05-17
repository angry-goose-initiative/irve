/* execute.h
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Utility functions for executing instructions
 *
*/

#ifndef EXECUTE_H
#define EXECUTE_H

/* Includes */

#include "cpu_state.h"
#include "memory.h"
#include "decode.h"
#include "CSR.h"

/* Function/Class Declarations */

namespace irve::internal::execute {
    void load    (const decode::decoded_inst_t& decoded_inst, cpu_state::cpu_state_t& cpu_state, memory::memory_t& memory, const CSR::CSR_t& CSR);
    void custom_0(const decode::decoded_inst_t& decoded_inst, cpu_state::cpu_state_t& cpu_state, memory::memory_t& memory,       CSR::CSR_t& CSR);
    void misc_mem(const decode::decoded_inst_t& decoded_inst, cpu_state::cpu_state_t& cpu_state,                           const CSR::CSR_t& CSR);
    void op_imm  (const decode::decoded_inst_t& decoded_inst, cpu_state::cpu_state_t& cpu_state,                           const CSR::CSR_t& CSR);
    void auipc   (const decode::decoded_inst_t& decoded_inst, cpu_state::cpu_state_t& cpu_state,                           const CSR::CSR_t& CSR);
    void store   (const decode::decoded_inst_t& decoded_inst, cpu_state::cpu_state_t& cpu_state, memory::memory_t& memory, const CSR::CSR_t& CSR);
    void amo     (const decode::decoded_inst_t& decoded_inst, cpu_state::cpu_state_t& cpu_state, memory::memory_t& memory, const CSR::CSR_t& CSR);
    void op      (const decode::decoded_inst_t& decoded_inst, cpu_state::cpu_state_t& cpu_state,                           const CSR::CSR_t& CSR);
    void lui     (const decode::decoded_inst_t& decoded_inst, cpu_state::cpu_state_t& cpu_state,                           const CSR::CSR_t& CSR);
    void branch  (const decode::decoded_inst_t& decoded_inst, cpu_state::cpu_state_t& cpu_state,                           const CSR::CSR_t& CSR);
    void jalr    (const decode::decoded_inst_t& decoded_inst, cpu_state::cpu_state_t& cpu_state,                           const CSR::CSR_t& CSR);
    void jal     (const decode::decoded_inst_t& decoded_inst, cpu_state::cpu_state_t& cpu_state,                           const CSR::CSR_t& CSR);
    void system  (const decode::decoded_inst_t& decoded_inst, cpu_state::cpu_state_t& cpu_state,                                 CSR::CSR_t& CSR);
}

#endif//EXECUTE_H
