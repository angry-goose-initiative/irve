/* CSR.cpp
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Performs unit tests for IRVE's CSR.h and CSR.cpp
 *
*/

/* Includes */

#include <cassert>
#include <cstddef>
#include "common.h"
#include "CSR.h"

using namespace irve::internal;

/* Function Implementations */

int test_CSR_CSR_t_init() {
    CSR::CSR_t csr;

    //Ensure we comply with section 3.4
    assert(csr.get_privilege_mode() == CSR::privilege_mode_t::MACHINE_MODE);//We must start in machine mode

    reg_t mstatus = csr.explicit_read(CSR::address::MSTATUS);
    assert(mstatus.bit(3) == 0);//mstatus.MIE must be 0
    assert(mstatus.bit(17) == 0);//mstatus.MPRV must be 0
    assert(csr.explicit_read(CSR::address::MSTATUSH).bit(5) == 0);//mstatush.MBE must be 0
    assert(csr.explicit_read(CSR::address::MISA) == 0);//misa must be 0 (or set to the maximally supported ISA if misa was implemented, but it isn't in IRVE)
    assert(csr.explicit_read(CSR::address::MCAUSE) == 0);//mcause must indicate the cause of the reset, but since IRVE dosn't differentiate between reset causes, it must be 0

    for (uint16_t i = CSR::address::PMPCFG_START; i <= CSR::address::PMPCFG_END; ++i) {
        reg_t pmpcfgi = csr.explicit_read(i);
        assert(pmpcfgi.bits(4, 3) == 0);//pmpcfgi.A must be 0
        assert(pmpcfgi.bit(7) == 0);//pmpcfgi.L must be 0
    }

    //TODO although other registers are undefined, ensure they still have legal values

    return 0;
}
