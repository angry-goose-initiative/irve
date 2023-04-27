/* NAME//TODO
 * By: John Jekel
 *
 * TODO description
 *
*/

#ifndef CPU_STATE_H
#define CPU_STATE_H

/* Includes */

#include <cstdint>

/* Constants And Defines */

//TODO

/* Types */

class cpu_state_t {
public:
    cpu_state_t();

    void retire_inst();
    uint64_t get_insts_retired() const;

    uint32_t get_pc() const;
    void set_pc(uint32_t new_pc);

    uint32_t get_r(uint8_t reg_num) const;
    void set_r(uint8_t reg_num, uint32_t new_val);

    //TODO stuff for setting interrupts
    //TODO CSRs

private:
    uint64_t insts_retired;
    uint32_t pc;
    uint32_t reg[31];

    //TODO registers, interrupts, CSRs, etc
};

/* Global Variables */

//TODO

/* Function/Class Declarations */

//TODO

#endif//CPU_STATE_H
