/* NAME//TODO
 * By: John Jekel & Nick Chan
 *
 * TODO description
 *
*/

#ifndef EXECUTE_H
#define EXECUTE_H

/* Includes */

#include "cpu_state.h"
#include "memory.h"
#include "decode.h"

/* Constants And Defines */

//TODO

/* Types */

//TODO

/* Global Variables */

//TODO

/* Function/Class Declarations */

void execute_load(const decoded_inst_t &decoded_inst, cpu_state_t &cpu_state, memory_t &memory);
void execute_misc_mem(const decoded_inst_t &decoded_inst, cpu_state_t &cpu_state, memory_t &memory);
void execute_op_imm(const decoded_inst_t &decoded_inst, cpu_state_t &cpu_state);
void execute_auipc(const decoded_inst_t &decoded_inst, cpu_state_t &cpu_state);
void execute_store(const decoded_inst_t &decoded_inst, cpu_state_t &cpu_state, memory_t &memory);
void execute_amo(const decoded_inst_t &decoded_inst, cpu_state_t &cpu_state, memory_t &memory);
void execute_op(const decoded_inst_t &decoded_inst, cpu_state_t &cpu_state);
void execute_lui(const decoded_inst_t &decoded_inst, cpu_state_t &cpu_state);
void execute_branch(const decoded_inst_t &decoded_inst, cpu_state_t &cpu_state);
void execute_jalr(const decoded_inst_t &decoded_inst, cpu_state_t &cpu_state);
void execute_jal(const decoded_inst_t &decoded_inst, cpu_state_t &cpu_state);
void execute_system(const decoded_inst_t &decoded_inst, cpu_state_t &cpu_state, memory_t &memory);

#endif//EXECUTE_H
