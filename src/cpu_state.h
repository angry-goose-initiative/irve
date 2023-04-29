/* NAME//TODO
 * By: John Jekel & Nick Chan
 *
 * TODO description
 *
*/

#ifndef CPU_STATE_H
#define CPU_STATE_H

/* Includes */

#include <cstdint>

#include "Reg.h"
#include "csrs.h"

/* Types */

typedef enum {
    USER_MODE = 0b00,
    SUPERVISOR_MODE = 0b01,
    MACHINE_MODE = 0b11
} privilege_mode_t;

/* Function/Class Declarations */

class cpu_state_t {
public:
    cpu_state_t();

    void increment_inst_count();
    uint64_t get_inst_count() const;

    uint32_t get_pc() const;
    void set_pc(uint32_t new_pc);

    Reg get_r(uint8_t reg_num) const;
    void set_r(uint8_t reg_num, uint32_t new_val);
    void set_r(uint8_t reg_num, int32_t new_val);

    void log(uint8_t indent) const;

    //TODO stuff for setting interrupts
    //TODO CSRs

private:
    privilege_mode_t m_privilege_mode;

    uint64_t m_inst_count;
    uint32_t m_pc;
    Reg m_regs[32];

    csrs_t m_csrs;

    //TODO registers, interrupts, CSRs, etc
};

#endif//CPU_STATE_H
