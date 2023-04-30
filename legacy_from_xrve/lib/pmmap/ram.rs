/* ram.rs
 * By: John Jekel
 *
 * Memory handler providing RAM for the RISC-V system
 *
*/

/* Imports */

use crate::logging::prelude::*;

use crate::pmmap::memory_handler::MemoryHandler;
use crate::pmmap::memory_handler::AccessType;
use crate::pmmap::memory_handler::AccessSize;
use crate::pmmap::memory_handler::MatchCriteria;
use crate::state::State;

/* Constants */

const RESET_CONTENTS: u8 = 0xA5;

/* Macros */

//TODO (also pub(crate) use the_macro statements here too)

/* Static Variables */

//TODO

/* Types */

pub struct Ram {
    start_addr: u32,
    backing_memory: Box<[u8]>,
    match_criteria: Vec<MatchCriteria>,
}

/* Associated Functions and Methods */

impl Ram {
    pub fn new(start_addr: u32, size: u32) -> Self {
        debug_assert!(size > 0, "Cannot create empty Ram");
        let mut new_ram = Self {
            start_addr: start_addr,
            backing_memory: vec![RESET_CONTENTS; size as usize].into_boxed_slice(),
            match_criteria: Vec::new()
        };

        new_ram.match_criteria.push(MatchCriteria::AddressRange(AccessType::Fetch, AccessSize::All, start_addr, new_ram.end_addr()));
        new_ram.match_criteria.push(MatchCriteria::AddressRange(AccessType::Read,  AccessSize::All, start_addr, new_ram.end_addr()));
        new_ram.match_criteria.push(MatchCriteria::AddressRange(AccessType::Write, AccessSize::All, start_addr, new_ram.end_addr()));

        new_ram
    }

    fn end_addr(&self) -> u32 {
        self.start_addr + (self.backing_memory.len() as u32)
    }
}

impl MemoryHandler for Ram {
    fn get_match_criteria(&self) -> Vec<MatchCriteria> {
        return self.match_criteria.clone();
    }

    fn read_byte(&mut self, state: &mut State, addr: u32, l: &mut Logger) -> u8 {
        debug_assert!((addr >= self.start_addr) && (addr < self.end_addr()), "Ram read out of bounds");
        let data = self.backing_memory[(addr - self.start_addr) as usize];
        log!(255, "Ram: Read 0x{:x} from address 0x{:x}", data, addr);
        data
    }

    fn write_byte(&mut self, state: &mut State, addr: u32, data: u8, l: &mut Logger) {
        debug_assert!((addr >= self.start_addr) && (addr < self.end_addr()), "Ram write out of bounds");
        log!(255, "Ram: Write 0x{:x} to address 0x{:x}", data, addr);
        self.backing_memory[(addr - self.start_addr) as usize] = data;
    }
}

/* Functions */

//TODO

/* Tests */

//TODO
