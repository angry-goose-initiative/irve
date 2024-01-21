/**
 * @brief   Utility functions for executing instructions
 * 
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  Copyright (C) 2023 Nick Chan\n
 *  See the LICENSE file at the root of the project for licensing info.
*/

#pragma once

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include "cpu_state.h"
#include "memory.h"
#include "decode.h"
#include "CSR.h"

/* ------------------------------------------------------------------------------------------------
 * Function Declarations
 * --------------------------------------------------------------------------------------------- */

/**
 * @brief The internal irve namespace for executing RISC-V instructions
 *
 * Split into functions by major (5-bit) opcode
*/
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
