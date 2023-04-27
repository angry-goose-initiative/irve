/* NAME//TODO
 * By: John Jekel
 *
 * TODO description
 *
*/

/* Imports */

use crate::logging::prelude::*;

//TODO (include "use" and "mod" here)
use crate::state::State;
use crate::pmmap::PhysicalMemoryMap;

/* Constants */

//TODO

/* Macros */

//TODO (also pub(crate) use the_macro statements here too)

/* Static Variables */

//TODO

/* Types */

//TODO
#[derive(Debug, Clone, Copy)]
pub enum RawInstruction {
    Regular(u32),
    Compressed(u16),
    Unaligned,
    Fault,
}

/* Associated Functions and Methods */

//TODO

/* Functions */

pub fn fetch_raw(state: &mut State, pmmap: &mut PhysicalMemoryMap, l: &mut Logger) -> RawInstruction {
    log!(129, "Fetching instruction from address 0x{:08x}", state.pc());

    //Ensure that the PC is aligned to a 2-byte boundary
    if (state.pc() % 2) != 0 {
        log!(130, "PC address wasn't aligned to a 2-byte boundary when fetching instruction");
        return RawInstruction::Unaligned;
    }

    //Get the first/only halfword of the instruction
    let first_halfword: u16;
    if let Ok(halfword) = pmmap.read_halfword(state, state.pc(), l) {
        log!(130, "Read first halfword of instruction: 0x{:04x}", halfword);
        first_halfword = halfword;
    } else {
        log!(130, "Access fault when reading first halfword of instruction");
        return RawInstruction::Fault;
    }

    //Depending on the lower to bits of the first halfword, determine if the instruction is compressed or not
    if (first_halfword & 0b11) == 0b11 {//Uncompressed
        log!(130, "Instruction is uncompressed");
        if let Ok(halfword) = pmmap.read_halfword(state, state.pc() + 2, l) {
            log!(130, "Read second halfword of instruction: 0x{:04x}", halfword);
            let word = ((halfword as u32) << 16) | (first_halfword as u32);
            return RawInstruction::Regular(word);
        } else {
            log!(130, "Access fault when reading second halfword of instruction");
            return RawInstruction::Fault;
        }
    } else {//Compressed (0b00, 0b01, 0b10)
        return RawInstruction::Compressed(first_halfword);
    }
}

/* Tests */

//TODO
