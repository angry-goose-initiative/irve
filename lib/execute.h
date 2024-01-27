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
#include "csr.h"

/* ------------------------------------------------------------------------------------------------
 * Function Declarations
 * --------------------------------------------------------------------------------------------- */

/**
 * @brief The internal irve namespace for executing RISC-V instructions
 *
 * Split into functions by major (5-bit) opcode
*/
namespace irve::internal::execute {
    void load    (const decode::decoded_inst_t& decoded_inst, CpuState& cpu_state, Memory& memory, Csr& CSR);
    void custom_0(const decode::decoded_inst_t& decoded_inst, CpuState& cpu_state, Memory& memory, Csr& CSR);
    void misc_mem(const decode::decoded_inst_t& decoded_inst, CpuState& cpu_state,                 Csr& CSR);
    void op_imm  (const decode::decoded_inst_t& decoded_inst, CpuState& cpu_state,                 Csr& CSR);
    void auipc   (const decode::decoded_inst_t& decoded_inst, CpuState& cpu_state,                 Csr& CSR);
    void store   (const decode::decoded_inst_t& decoded_inst, CpuState& cpu_state, Memory& memory, Csr& CSR);
    void amo     (const decode::decoded_inst_t& decoded_inst, CpuState& cpu_state, Memory& memory, Csr& CSR);
    void op      (const decode::decoded_inst_t& decoded_inst, CpuState& cpu_state,                 Csr& CSR);
    void lui     (const decode::decoded_inst_t& decoded_inst, CpuState& cpu_state,                 Csr& CSR);
    void branch  (const decode::decoded_inst_t& decoded_inst, CpuState& cpu_state,                 Csr& CSR);
    void jalr    (const decode::decoded_inst_t& decoded_inst, CpuState& cpu_state,                 Csr& CSR);
    void jal     (const decode::decoded_inst_t& decoded_inst, CpuState& cpu_state,                 Csr& CSR);
    void system  (const decode::decoded_inst_t& decoded_inst, CpuState& cpu_state,                 Csr& CSR);
}
