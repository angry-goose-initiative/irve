/* decode.cpp
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Performs unit tests for IRVE's decode.h and decode.cpp
 *
*/

/* Includes */

#include <cassert>
#include <cstdint>
#include "decode.h"

#include "rvexception.h"

using namespace irve::internal;

/* Function Implementations */

int test_decode_decoded_inst_t() {
    decode::decoded_inst_t nop(0x00000013);
    assert(nop.get_opcode() == OP_IMM);
    assert(nop.get_rd() == 0);
    assert(nop.get_funct3() == 0);
    assert(nop.get_rs1() == 0);
    assert(nop.get_imm() == 0);

    //TODO more tests

    return 0;
}

int test_decode_decoded_inst_t_invalid() {
    //All 0s is an invalid instruction
    try {
        decode::decoded_inst_t invalid(0x00000000);
        assert(false);
    } catch (const rvexception_t& e) {
        assert(e.cause() == cause_t::ILLEGAL_INSTRUCTION_EXCEPTION);
    }

    //All 1s is an invalid instruction
    try {
        decode::decoded_inst_t invalid(0xFFFFFFFF);
        assert(false);
    } catch (const rvexception_t& e) {
        assert(e.cause() == cause_t::ILLEGAL_INSTRUCTION_EXCEPTION);
    }

    //We don't support compressed instructions
    try {
        decode::decoded_inst_t invalid(0x00000001);
        assert(false);
    } catch (const rvexception_t& e) {
        assert(e.cause() == cause_t::ILLEGAL_INSTRUCTION_EXCEPTION);
    }

    //TODO more tests

    return 0;
}
