/**
 * @brief   M-Mode semihosting support for IRVE
 * 
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  See the LICENSE file at the root of the project for licensing info.
 * 
 * TODO longer description
 *
*/

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include "semihosting.h"

#include "cpu_state.h"
#include "memory.h"

#define INST_COUNT 0
#include "logging.h"

using namespace irve::internal;

/* ------------------------------------------------------------------------------------------------
 * Constants/Defines
 * --------------------------------------------------------------------------------------------- */

constexpr uint8_t a0{10}; // FIXME - These should be replaced with a reg select enum
constexpr uint8_t a1{11};

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

SemihostingHandler::~SemihostingHandler() {
    if (this->m_output_line_buffer.size() > 0) {
        irvelog_always_stdout(
            0,
            "\x1b[92mRVSEMIHOSTING:\x1b[0m: \"\x1b[1m%s\x1b[0m\"",
            this->m_output_line_buffer.c_str()
        );
    }
}

void SemihostingHandler::handle(CpuState& cpu_state, Memory& memory/*, const Csr& CSR*/) {
    //TODO don't use always stdout for everything
    switch (cpu_state.get_r(a0).u) {
        case 0x03: {//SYS_WRITEC
            //TODO actually save this into some buffer and print out a whole line at a time
            Word char_pointer = cpu_state.get_r(a1);
            try {
                char character = memory.load(char_pointer, 0b000).u;
                switch (character) {
                    case '\n':
                        //End of line; print the contents of the line buffer and clear it
                        irvelog_always_stdout(
                            0,
                            "\x1b[92mRVSEMIHOSTING\x1b[0m: \"\x1b[1m%s\x1b[0m\\n\"",
                            this->m_output_line_buffer.c_str()
                        );
                        this->m_output_line_buffer.clear();
                        break;
                    case '\0':
                        //Null terminator; print the contents of the line buffer and clear it
                        //(this has helped with debugging weird issues in the past)
                        irvelog_always_stdout(
                            0,
                            "\x1b[92mRVSEMIHOSTING\x1b[0m: \"\x1b[1m%s\x1b[0m\\0\"",
                            this->m_output_line_buffer.c_str()
                        );
                        this->m_output_line_buffer.clear();
                        break;
                    case '\r':  this->m_output_line_buffer += "\x1b[0m\\r\x1b[1m"; break;//Print \r in non-bold
                    default:    this->m_output_line_buffer.push_back(character); break;
                }
            } catch (const rv_trap::rvexception_t&) {
                irvelog(0, "SYS_WRITEC: Invalid address 0x%08x", char_pointer);
            }
            break;
        }
        default:
            assert(false && "TODO implement other semihosting calls");
            irvelog(0, "Semihosting call 0x%08x not implemented\n", cpu_state.get_r(a0));//TODO should this be always?
            break;
    }
    cpu_state.goto_next_sequential_pc();
}
