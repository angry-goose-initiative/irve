/**
 * @file    decode.cpp
 * @brief   Performs unit tests for IRVE's decode.h and decode.cpp
 * 
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  See the LICENSE file at the root of the project for licensing info.
*/

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include <cassert>
#include <cstdint>
#include "decode.h"

#include "rv_trap.h"

using namespace irve::internal;

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

int test_decode_decoded_inst_t() {
    decode::DecodedInst nop(0x00000013);
    assert(nop.get_opcode() == decode::Opcode::OP_IMM);
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
        decode::DecodedInst invalid(0x00000000);
        assert(false);
    } catch (const rv_trap::RvException& e) {
        assert(e.cause() == rv_trap::Cause::ILLEGAL_INSTRUCTION_EXCEPTION);
    }

    //All 1s is an invalid instruction
    try {
        decode::DecodedInst invalid(0xFFFFFFFF);
        assert(false);
    } catch (const rv_trap::RvException& e) {
        assert(e.cause() == rv_trap::Cause::ILLEGAL_INSTRUCTION_EXCEPTION);
    }

    //We don't support compressed instructions
    try {
        decode::DecodedInst invalid(0x00000001);
        assert(false);
    } catch (const rv_trap::RvException& e) {
        assert(e.cause() == rv_trap::Cause::ILLEGAL_INSTRUCTION_EXCEPTION);
    }

    //TODO more tests

    return 0;
}
