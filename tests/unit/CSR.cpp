/**
 * @file    CSR.cpp
 * @brief   Performs unit tests for IRVE's CSR.h and CSR.cpp
 * 
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  See the LICENSE file at the root of the project for licensing info.
*/

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include <cassert>
#include <cstddef>
#include "common.h"
#include "CSR.h"

using namespace irve::internal;

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

int test_CSR_Csr_init() {
    Csr csr;

    //Ensure we comply with section 3.4
    assert(csr.get_privilege_mode() == PrivilegeMode::MACHINE_MODE);//We must start in machine mode

    Reg mstatus = csr.explicit_read(Csr::Address::MSTATUS);
    assert(mstatus.bit(3) == 0);//mstatus.MIE must be 0
    assert(mstatus.bit(17) == 0);//mstatus.MPRV must be 0
    assert(csr.explicit_read(Csr::Address::MSTATUSH).bit(5) == 0);//mstatush.MBE must be 0
    assert(csr.explicit_read(Csr::Address::MISA) == 0);//misa must be 0 (or set to the maximally supported ISA if misa was implemented, but it isn't in IRVE)
    assert(csr.explicit_read(Csr::Address::MCAUSE) == 0);//mcause must indicate the cause of the reset, but since IRVE dosn't differentiate between reset causes, it must be 0

    for (auto i = static_cast<uint16_t>(Csr::Address::PMPCFG_START); i <= static_cast<uint16_t>(Csr::Address::PMPCFG_END); ++i) {
        Reg pmpcfgi = csr.explicit_read(static_cast<Csr::Address>(i));
        assert(pmpcfgi.bits(4, 3) == 0);//pmpcfgi.A must be 0
        assert(pmpcfgi.bit(7) == 0);//pmpcfgi.L must be 0
    }

    //TODO although other registers are undefined, ensure they still have legal values

    return 0;
}
