/* ram.rs
 * By: John Jekel
 *
 * Memory handler providing RAM for the RISC-V system
 *
*/

/* Imports */

use crate::pmmap::memory_handler::MemoryHandler;
use crate::pmmap::memory_handler::MatchCriteria;
use crate::state::State;

/* Constants */

//TODO

/* Macros */

//TODO (also pub(crate) use the_macro statements here too)

/* Static Variables */

//TODO

/* Types */

pub struct Ram {
    start_addr: u32,
    end_addr: u32
}

/* Associated Functions and Methods */

impl Ram {
    pub fn new(start_addr: u32, end_addr: u32) -> Self {
        Self {
            start_addr,
            end_addr
        }
    }
}

impl MemoryHandler for Ram {
    fn get_match_criteria(&self) -> &[MatchCriteria] {
        todo!();
    }

    fn read_byte(&self, state: &mut State, addr: u32) -> u8 {
        todo!();
    }

    fn write_byte(&self, state: &mut State, addr: u32, data: u8) {
        todo!();
    }
}

/* Functions */

//TODO
