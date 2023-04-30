/* pmmap.rs
 * By: John Jekel
 *
 * Physical Memory Map for XRVE
 * (holds memory handlers and manages memory accesses)
 *
*/

/* Imports */

pub mod memory_handler;
pub mod ram;

use crate::logging::prelude::*;

use crate::state::State;
use memory_handler::MemoryHandler;
use memory_handler::AccessType;
use memory_handler::AccessSize;

/* Constants */

//TODO

/* Macros */

//TODO (also pub(crate) use the_macro statements here too)

/* Static Variables */

//TODO

/* Types */

pub struct PhysicalMemoryMap {
    handlers: Vec<Box<dyn MemoryHandler + Send>>,
}

/* Associated Functions and Methods */

impl PhysicalMemoryMap {
    pub fn new() -> Self {
        Self {
            handlers: Vec::new(),
        }
    }

    //NOTE: 'static in the context of impl DOSN'T mean the handler will live forever
    //It just means it has no non-static references that could be destroyed while we still possess
    //it. It could still have references to static things (like strings) or can hold a Sender
    //so that it can communicate with the user code that created it
    //This stops the user from ex. passing a reference to a struct where the reference is impl MemoryHandler
    pub fn register_handler(&mut self, handler: impl MemoryHandler + Send + 'static) {
        //TODO ensure no conflicts with existing handlers
        self.handlers.push(Box::new(handler));
    }

    //TODO functions for reading, writing, and fetching (which will lookup the proper handler)
    pub fn fetch_byte(&mut self, state: &mut State, addr: u32, l: &mut Logger) -> Result<u8, ()> {
        Ok(self.search(addr, AccessType::Fetch, AccessSize::Byte)?.fetch_byte(state, addr, l))
    }
    pub fn read_byte(&mut self, state: &mut State, addr: u32, l: &mut Logger) -> Result<u8, ()> {
        Ok(self.search(addr, AccessType::Read, AccessSize::Byte)?.read_byte(state, addr, l))
    }
    pub fn write_byte(&mut self, state: &mut State, addr: u32, data: u8, l: &mut Logger) -> Result<(), ()> {
        self.search(addr, AccessType::Write, AccessSize::Byte)?.write_byte(state, addr, data, l);
        Ok(())
    }

    //TODO what if we can only legally access the first byte in a multi-byte access?
    //How do we tell if we're allowed to access the rest of the bytes?
    //TODO disallow unaligned accesses
    pub fn fetch_halfword(&mut self, state: &mut State, addr: u32, l: &mut Logger) -> Result<u16, ()> {
        Ok(self.search(addr, AccessType::Fetch, AccessSize::Halfword)?.fetch_halfword(state, addr, l))
    }
    pub fn read_halfword(&mut self, state: &mut State, addr: u32, l: &mut Logger) -> Result<u16, ()> {
        Ok(self.search(addr, AccessType::Read, AccessSize::Halfword)?.read_halfword(state, addr, l))
    }
    pub fn write_halfword(&mut self, state: &mut State, addr: u32, data: u16, l: &mut Logger) -> Result<(), ()> {
        self.search(addr, AccessType::Write, AccessSize::Halfword)?.write_halfword(state, addr, data, l);
        Ok(())
    }
    pub fn fetch_word(&mut self, state: &mut State, addr: u32, l: &mut Logger) -> Result<u32, ()> {
        Ok(self.search(addr, AccessType::Fetch, AccessSize::Word)?.fetch_word(state, addr, l))
    }
    pub fn read_word(&mut self, state: &mut State, addr: u32, l: &mut Logger) -> Result<u32, ()> {
        Ok(self.search(addr, AccessType::Read, AccessSize::Word)?.read_word(state, addr, l))
    }
    pub fn write_word(&mut self, state: &mut State, addr: u32, data: u32, l: &mut Logger) -> Result<(), ()> {
        self.search(addr, AccessType::Write, AccessSize::Word)?.write_word(state, addr, data, l);
        Ok(())
    }

    fn search(&mut self, addr: u32, access_type: AccessType, access_size: AccessSize) -> Result<&mut Box<dyn MemoryHandler + Send>, ()> {
        //NOTE: We don't have to worry about accesses crossing multiple handlers or a handler and unmapped memory
        //because the MatchCriteria AddressRange variant requires that the size of the range be a multiple of the access size

        //TODO do this in a more efficient way than linear search

        //Bail out if an unaligned access is attempted (satisfied will panic if the access is unaligned)
        match access_size {
            AccessSize::All => panic!("All is disallowed as a search criterion"),//TODO replace with debug panic
            AccessSize::Byte => {},//Byte accesses have no restrictions
            AccessSize::Halfword => if (addr & 0x1) != 0 { return Err(()); },
            AccessSize::Word => if (addr & 0x3) != 0 { return Err(()); },
        }
        
        //Linearly search the handlers for a match
        for handler in &mut self.handlers {
            //Linearly search the criteria in each handler for a match
            for criterion in handler.get_match_criteria() {
                if criterion.satisfies(addr, access_type, access_size) {
                    return Ok(handler);
                }
            }
        }

        Err(())
    }
}

/* Functions */

//TODO

/* Tests */

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn create_pmmap() {
        let _ = PhysicalMemoryMap::new();
    }

    #[test]
    fn register_handler() {
        let mut pmmap = PhysicalMemoryMap::new();

        pmmap.register_handler(ram::Ram::new(0x1234, 0x5678));
    }

    #[test]
    fn search_sanity() {
        let mut pmmap = PhysicalMemoryMap::new();

        assert!(pmmap.search(0x1238, AccessType::Read, AccessSize::Byte).is_err());
        assert!(pmmap.search(0x123C, AccessType::Read, AccessSize::Halfword).is_err());
        assert!(pmmap.search(0x1244, AccessType::Read, AccessSize::Word).is_err());
        assert!(pmmap.search(0x1248, AccessType::Write, AccessSize::Byte).is_err());
        assert!(pmmap.search(0x124C, AccessType::Write, AccessSize::Halfword).is_err());
        assert!(pmmap.search(0x1250, AccessType::Write, AccessSize::Word).is_err());

        pmmap.register_handler(ram::Ram::new(0x1234, 0x5678));

        assert!(pmmap.search(0x1238, AccessType::Read, AccessSize::Byte).is_ok());
        assert!(pmmap.search(0x123C, AccessType::Read, AccessSize::Halfword).is_ok());
        assert!(pmmap.search(0x1240, AccessType::Read, AccessSize::Word).is_ok());
        assert!(pmmap.search(0x1248, AccessType::Write, AccessSize::Byte).is_ok());
        assert!(pmmap.search(0x124C, AccessType::Write, AccessSize::Halfword).is_ok());
        assert!(pmmap.search(0x1250, AccessType::Write, AccessSize::Word).is_ok());

        assert!(pmmap.search(0x1000, AccessType::Read, AccessSize::Byte).is_err());
        assert!(pmmap.search(0x1000, AccessType::Read, AccessSize::Halfword).is_err());
        assert!(pmmap.search(0x1000, AccessType::Read, AccessSize::Word).is_err());
        assert!(pmmap.search(0x1000, AccessType::Write, AccessSize::Byte).is_err());
        assert!(pmmap.search(0x1000, AccessType::Write, AccessSize::Halfword).is_err());
        assert!(pmmap.search(0x1000, AccessType::Write, AccessSize::Word).is_err());
    }

    #[test]
    fn search_unaligned() {
        let mut pmmap = PhysicalMemoryMap::new();

        assert!(pmmap.search(0x123B, AccessType::Read, AccessSize::Halfword).is_err());
        assert!(pmmap.search(0x123F, AccessType::Read, AccessSize::Word).is_err());
        assert!(pmmap.search(0x1247, AccessType::Write, AccessSize::Halfword).is_err());
        assert!(pmmap.search(0x124B, AccessType::Write, AccessSize::Word).is_err());

        pmmap.register_handler(ram::Ram::new(0x1234, 0x5678));

        assert!(pmmap.search(0x123B, AccessType::Read, AccessSize::Halfword).is_err());
        assert!(pmmap.search(0x123F, AccessType::Read, AccessSize::Word).is_err());
        assert!(pmmap.search(0x1247, AccessType::Write, AccessSize::Halfword).is_err());
        assert!(pmmap.search(0x124B, AccessType::Write, AccessSize::Word).is_err());

        assert!(pmmap.search(0x1001, AccessType::Read, AccessSize::Halfword).is_err());
        assert!(pmmap.search(0x1001, AccessType::Read, AccessSize::Word).is_err());
        assert!(pmmap.search(0x1001, AccessType::Write, AccessSize::Halfword).is_err());
        assert!(pmmap.search(0x1001, AccessType::Write, AccessSize::Word).is_err());
    }

    //TODO more tests
}
