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

/* Constants And Defines */

//TODO

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

    Reg get_CSR(uint16_t CSR_num) const;
    void set_CSR(uint16_t CSR_num, uint32_t new_val);
    // TODO add a verstion of set for signed?

    void log(uint8_t indent) const;

    //TODO stuff for setting interrupts
    //TODO CSRs

private:
    uint64_t m_inst_count;
    uint32_t m_pc;
    Reg m_regs[32];
    Reg m_CSR[4096];
    uint8_t m_privilege_level; // TODO how is this changed?

    //TODO registers, interrupts, CSRs, etc
};

#endif//CPU_STATE_H
