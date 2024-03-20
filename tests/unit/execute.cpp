/**
 * @file    execute.cpp
 * @brief   Performs unit tests for IRVE's execute.cpp
 * 
 * @copyright
 *  Copyright (C) 2024 John Jekel\n
 *  See the LICENSE file at the root of the project for licensing info.
*/

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#define private public//Since we need to access internal emulator state for testing

#undef NDEBUG//Asserts should work even in release mode for tests
#include <cassert>

#include <cstdint>

#include "irve_public_api.h"
#include "emulator.h"
#include "cpu_state.h"
#include "csr.h"
#include "execute.h"

using namespace irve::internal;

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

int test_execute_high_multiply() {
    irve::emulator::emulator_t  emulator(0, nullptr);
    irve::internal::CpuState&   cpu_state_ref = emulator.m_emulator_ptr->m_cpu_state;
    irve::internal::Csr&        csr_ref = emulator.m_emulator_ptr->m_CSR;

    //mulh x1, x2, x3
    decode::DecodedInst mulh(0x023110b3);
    cpu_state_ref.set_r(2, 0x7FFFFFFF);//Both signed positive
    cpu_state_ref.set_r(3, 0x7FFFFFFF);
    irve::internal::execute::op(mulh, cpu_state_ref, csr_ref);
    assert(cpu_state_ref.get_r(1) == 0x3FFFFFFF);//Result is positive
    cpu_state_ref.set_r(2, 0xFFFFFFFF);//This is signed negative
    cpu_state_ref.set_r(3, 0x7FFFFFFF);//This is signed positive
    irve::internal::execute::op(mulh, cpu_state_ref, csr_ref);
    assert(cpu_state_ref.get_r(1) == 0xFFFFFFFF);//Negated 0x7FFFFFFF still only takes 32 bits so the upper bits are all 1s
    cpu_state_ref.set_r(2, 0x7FFFFFFF);//This is signed negative
    cpu_state_ref.set_r(3, 0xFFFFFFFF);//This is signed positive
    irve::internal::execute::op(mulh, cpu_state_ref, csr_ref);
    assert(cpu_state_ref.get_r(1) == 0xFFFFFFFF);//Negated 0x7FFFFFFF still only takes 32 bits so the upper bits are all 1s

    //Added as an extra for mulh since Eric was seeing issues with this example
    cpu_state_ref.set_r(2, -100);
    cpu_state_ref.set_r(3, 10);
    irve::internal::execute::op(mulh, cpu_state_ref, csr_ref);
    assert(cpu_state_ref.get_r(1) == 0xFFFFFFFF);

    //mulhsu x1, x2, x3
    decode::DecodedInst mulhsu(0x023120b3);
    cpu_state_ref.set_r(2, 0x7FFFFFFF);//Signed positive
    cpu_state_ref.set_r(3, 0x7FFFFFFF);//Unsigned positive
    irve::internal::execute::op(mulhsu, cpu_state_ref, csr_ref);
    assert(cpu_state_ref.get_r(1) == 0x3FFFFFFF);//Result is positive
    cpu_state_ref.set_r(2, 0xFFFFFFFF);//This is signed negative
    cpu_state_ref.set_r(3, 0x7FFFFFFF);//This is unsigned positive
    irve::internal::execute::op(mulhsu, cpu_state_ref, csr_ref);
    assert(cpu_state_ref.get_r(1) == 0xFFFFFFFF);//Negated 0x7FFFFFFF still only takes 32 bits so the upper bits are all 1s
    cpu_state_ref.set_r(2, 0x7FFFFFFF);//This is signed positive
    cpu_state_ref.set_r(3, 0xFFFFFFFF);//This is unsigned, so it's actually positive
    irve::internal::execute::op(mulhsu, cpu_state_ref, csr_ref);
    assert(cpu_state_ref.get_r(1) == 0x7FFFFFFE);//Result is a big positive number

    //mulhu x1, x2, x3
    decode::DecodedInst mulhu(0x023130b3);
    cpu_state_ref.set_r(2, 0x7FFFFFFF);//Unsigned positive
    cpu_state_ref.set_r(3, 0x7FFFFFFF);//Unsigned positive
    irve::internal::execute::op(mulhu, cpu_state_ref, csr_ref);
    assert(cpu_state_ref.get_r(1) == 0x3FFFFFFF);//Result is positive
    cpu_state_ref.set_r(2, 0xFFFFFFFF);//This is unsigned, so it's actually positive
    cpu_state_ref.set_r(3, 0x7FFFFFFF);//This is unsigned positive
    irve::internal::execute::op(mulhu, cpu_state_ref, csr_ref);
    assert(cpu_state_ref.get_r(1) == 0x7FFFFFFE);//Result is a big positive number
    cpu_state_ref.set_r(2, 0x7FFFFFFF);//This is signed positive
    cpu_state_ref.set_r(3, 0xFFFFFFFF);//This is unsigned, so it's actually positive
    irve::internal::execute::op(mulhu, cpu_state_ref, csr_ref);
    assert(cpu_state_ref.get_r(1) == 0x7FFFFFFE);//Result is a big positive number

    //TODO be more thorough with testing

    return 0;
}
