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
    void load(const decoded_inst_t& decoded_inst, cpu_state_t& cpu_state, memory_t& memory);
    void custom_0(const decoded_inst_t& decoded_inst, cpu_state_t& cpu_state, memory_t& memory, CSR::CSR_t& CSR);
    void misc_mem(const decoded_inst_t& decoded_inst, cpu_state_t& cpu_state);
    void op_imm(const decoded_inst_t& decoded_inst, cpu_state_t& cpu_state);
    void auipc(const decoded_inst_t& decoded_inst, cpu_state_t& cpu_state);
    void store(const decoded_inst_t& decoded_inst, cpu_state_t& cpu_state, memory_t& memory);
    void amo(const decoded_inst_t& decoded_inst, cpu_state_t& cpu_state, memory_t& memory);
    void op(const decoded_inst_t& decoded_inst, cpu_state_t& cpu_state);
    void lui(const decoded_inst_t& decoded_inst, cpu_state_t& cpu_state);
    void branch(const decoded_inst_t& decoded_inst, cpu_state_t& cpu_state);
    void jalr(const decoded_inst_t& decoded_inst, cpu_state_t& cpu_state);
    void jal(const decoded_inst_t& decoded_inst, cpu_state_t& cpu_state);
    void system(const decoded_inst_t& decoded_inst, cpu_state_t& cpu_state, CSR::CSR_t& CSR);
}

#endif//EXECUTE_H
