/* memory_handler.rs
 * By: John Jekel
 *
 * Like instruction handler, but for PHYSICAL memory addresses
 *
*/

/* Imports */

use crate::state::State;

/* Constants */

//TODO

/* Macros */

//TODO (also pub(crate) use the_macro statements here too)

/* Static Variables */

//TODO

/* Types */

//pub struct InstructionHandler {
    //TODO
//}
//

#[derive(Debug, Clone, Copy)]
pub enum AccessType {
    All,
    Fetch,
    Read,
    Write
}

#[derive(Debug, Clone, Copy)]
pub enum AccessSize {
    All,
    Byte,
    Halfword,
    Word
}

#[derive(Debug, Clone, Copy)]
pub enum MatchCriteria {
    Always(AccessType, AccessSize),
    SingleAddress(AccessType, AccessSize, u32),
    AddressRange(AccessType, AccessSize, u32, u32),//Inclusive
    Never

    //TODO
}

//TODO may need a lifetime parameter here for the callback
//IMPORTANT: Actually, no, the InstructionHandler will be consumed when it is registered
//and if the user wants to communicate with their own code, they can use a channel
pub trait MemoryHandler /*<const NUM_CRITERIA: usize>*/ {
    //const MATCH_CRITERIA: MatchCriteria;//[MatchCriteria; NUM_CRITERIA];
   
    fn get_match_criteria(&self) -> Vec<MatchCriteria>;

    //TODO add a function to get an identifying string

    //Memory handlers are allowed to panic if they are unable to handle the request
    //So the match criteria should be checked before calling the handler functions
    fn fetch_byte(&mut self, state: &mut State, addr: u32) -> u8 {
        self.read_byte(state, addr)//By default just implement fetch with read
    }
    fn read_byte(&mut self, state: &mut State, addr: u32) -> u8;
    fn write_byte(&mut self, state: &mut State, addr: u32, data: u8);

    //The functions that read multiple bytes MUST BE ALIGNED or else they may panic
    fn fetch_halfword(&mut self, state: &mut State, addr: u32) -> u16 {
        self.read_halfword(state, addr)//By default just implement fetch with read
    }
    fn read_halfword(&mut self, state: &mut State, addr: u32) -> u16 {
        //By default just implement read_halfword with read_byte (LITTLE ENDIAN)
        let mut data = self.read_byte(state, addr) as u16;
        data |= (self.read_byte(state, addr + 1) as u16) << 8;
        data
    }
    fn write_halfword(&mut self, state: &mut State, addr: u32, data: u16) {
        //By default just implement write_halfword with write_byte (LITTLE ENDIAN)
        self.write_byte(state, addr, data as u8);
        self.write_byte(state, addr + 1, (data >> 8) as u8);
    }
    fn fetch_word(&mut self, state: &mut State, addr: u32) -> u32 {
        self.read_word(state, addr)//By default just implement fetch with read
    }
    fn read_word(&mut self, state: &mut State, addr: u32) -> u32 {
        //By default just implement read_word with read_byte (LITTLE ENDIAN)
        let mut data = self.read_byte(state, addr) as u32;
        data |= (self.read_byte(state, addr + 1) as u32) << 8;
        data |= (self.read_byte(state, addr + 2) as u32) << 16;
        data |= (self.read_byte(state, addr + 3) as u32) << 24;
        data
    }
    fn write_word(&mut self, state: &mut State, addr: u32, data: u32) {
        //By default just implement write_word with write_byte (LITTLE ENDIAN)
        self.write_byte(state, addr, data as u8);
        self.write_byte(state, addr + 1, (data >> 8) as u8);
        self.write_byte(state, addr + 2, (data >> 16) as u8);
        self.write_byte(state, addr + 3, (data >> 24) as u8);
    }
    
    //TODO
}

/* Associated Functions and Methods */

impl AccessType {
    fn satisfies(&self, other: &Self) -> bool {
        use AccessType::*;

        match (self, other) {
            (All, _) => true,
            (Fetch, Fetch) => true,
            (Read, Read) => true,
            (Write, Write) => true,
            _ => false
        }
    }
}

impl AccessSize {
    fn satisfies(&self, other: &Self) -> bool {
        use AccessSize::*;

        match (self, other) {
            (All, _) => true,
            (Byte, Byte) => true,
            (Halfword, Halfword) => true,
            (Word, Word) => true,
            _ => false
        }
    }
}

impl MatchCriteria {
    pub fn satisfies(&self, addr: u32, access_type: AccessType, access_size: AccessSize) -> bool {
        use MatchCriteria::*;

        match self {
            Always(match_access_type, match_access_size)
                => (match_access_type.satisfies(&access_type)) && (match_access_size.satisfies(&access_size)),
            SingleAddress(match_access_type, match_access_size, match_addr)
                => (match_access_type.satisfies(&access_type)) && (match_access_size.satisfies(&access_size)) && (addr == *match_addr),
            AddressRange(match_access_type, match_access_size, match_start_addr, match_end_addr)
                => (match_access_type.satisfies(&access_type)) && (match_access_size.satisfies(&access_size)) && (addr >= *match_start_addr) && (addr <= *match_end_addr),
            _
                => false
        }
    }
}

/* Functions */

//TODO

/* Tests */

//TODO
