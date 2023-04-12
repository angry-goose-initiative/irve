/* NAME//TODO
 * By: John Jekel
 *
 * TODO description
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
    Fetch,
    Read,
    Write
}

//TODO add access size as well

#[derive(Debug, Clone, Copy)]
pub enum MatchCriteria {
    Always(AccessType),
    SingleAddress(AccessType, u32),
    AddressRange(AccessType, u32, u32),//Inclusive
    Never

    //TODO
}

//TODO may need a lifetime parameter here for the callback
//IMPORTANT: Actually, no, the InstructionHandler will be consumed when it is registered
//and if the user wants to communicate with their own code, they can use a channel
pub trait MemoryHandler /*<const NUM_CRITERIA: usize>*/ {
    //const MATCH_CRITERIA: MatchCriteria;//[MatchCriteria; NUM_CRITERIA];
   
    fn get_match_criteria(&self) -> &[MatchCriteria];

    //Memory handlers are allowed to panic if they are unable to handle the request
    //So the match criteria should be checked before calling the handler functions
    fn fetch_byte(&self, state: &mut State, addr: u32) -> u8 {
        self.read_byte(state, addr)//By default just implement fetch with read
    }
    fn read_byte(&self, state: &mut State, addr: u32) -> u8;
    fn write_byte(&self, state: &mut State, addr: u32, data: u8);

    //The functions that read multiple bytes MUST BE ALIGNED or else they may panic
    fn fetch_halfword(&self, state: &mut State, addr: u32) -> u16 {
        self.read_halfword(state, addr)//By default just implement fetch with read
    }
    fn read_halfword(&self, state: &mut State, addr: u32) -> u16 {
        //By default just implement read_halfword with read_byte (LITTLE ENDIAN)
        let mut data = self.read_byte(state, addr) as u16;
        data |= (self.read_byte(state, addr + 1) as u16) << 8;
        data
    }
    fn write_halfword(&self, state: &mut State, addr: u32, data: u16) {
        //By default just implement write_halfword with write_byte (LITTLE ENDIAN)
        self.write_byte(state, addr, data as u8);
        self.write_byte(state, addr + 1, (data >> 8) as u8);
    }
    fn fetch_word(&self, state: &mut State, addr: u32) -> u32 {
        self.read_word(state, addr)//By default just implement fetch with read
    }
    fn read_word(&self, state: &mut State, addr: u32) -> u32 {
        //By default just implement read_word with read_byte (LITTLE ENDIAN)
        let mut data = self.read_byte(state, addr) as u32;
        data |= (self.read_byte(state, addr + 1) as u32) << 8;
        data |= (self.read_byte(state, addr + 2) as u32) << 16;
        data |= (self.read_byte(state, addr + 3) as u32) << 24;
        data
    }
    fn write_word(&self, state: &mut State, addr: u32, data: u32) {
        //By default just implement write_word with write_byte (LITTLE ENDIAN)
        self.write_byte(state, addr, data as u8);
        self.write_byte(state, addr + 1, (data >> 8) as u8);
        self.write_byte(state, addr + 2, (data >> 16) as u8);
        self.write_byte(state, addr + 3, (data >> 24) as u8);
    }
    
    //TODO
}

/* Associated Functions and Methods */

impl MatchCriteria {
    pub fn matches(&self, addr: u32, access_type: AccessType) -> bool {
        match self {
            MatchCriteria::Always(access_type)              => true,
            MatchCriteria::SingleAddress(access_type, a)    => addr == *a,
            MatchCriteria::AddressRange(access_type, a, b)  => (addr >= *a) && (addr <= *b),
            default                                         => false
        }
    }
}

/* Functions */

//TODO
