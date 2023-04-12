/* NAME//TODO
 * By: John Jekel
 *
 * TODO description
 *
 * TODO this will be the struct that holds memory handlers for physical memory
 *
*/

/* Imports */

pub mod memory_handler;
pub mod ram;

use crate::state::State;
use memory_handler::MemoryHandler;
use memory_handler::AccessType;

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
        self.handlers.push(Box::new(handler));
    }

    //TODO functions for reading, writing, and fetching (which will lookup the proper handler)
    pub fn fetch_byte(&self, state: &mut State, addr: u32) -> Result<u8, ()> {
        Ok(self.search(addr, AccessType::Fetch)?.fetch_byte(state, addr))
    }
    pub fn read_byte(&self, state: &mut State, addr: u32) -> Result<u8, ()> {
        Ok(self.search(addr, AccessType::Read)?.read_byte(state, addr))
    }
    pub fn write_byte(&self, state: &mut State, addr: u32, data: u8) -> Result<(), ()> {
        self.search(addr, AccessType::Write)?.write_byte(state, addr, data);
        Ok(())
    }
    //TODO what if we can only legally access the first byte in a multi-byte access?
    //How do we tell if we're allowed to access the rest of the bytes?
    pub fn fetch_halfword(&self, state: &mut State, addr: u32) -> Result<u16, ()> {
        Ok(self.search(addr, AccessType::Fetch)?.fetch_halfword(state, addr))
    }
    pub fn read_halfword(&self, state: &mut State, addr: u32) -> Result<u16, ()> {
        Ok(self.search(addr, AccessType::Read)?.read_halfword(state, addr))
    }
    pub fn write_halfword(&self, state: &mut State, addr: u32, data: u16) -> Result<(), ()> {
        self.search(addr, AccessType::Write)?.write_halfword(state, addr, data);
        Ok(())
    }
    pub fn fetch_word(&self, state: &mut State, addr: u32) -> Result<u32, ()> {
        Ok(self.search(addr, AccessType::Fetch)?.fetch_word(state, addr))
    }
    pub fn read_word(&self, state: &mut State, addr: u32) -> Result<u32, ()> {
        Ok(self.search(addr, AccessType::Read)?.read_word(state, addr))
    }
    pub fn write_word(&self, state: &mut State, addr: u32, data: u32) -> Result<(), ()> {
        self.search(addr, AccessType::Write)?.write_word(state, addr, data);
        Ok(())
    }

    fn search(&self, addr: u32, access_type: AccessType) -> Result<&Box<dyn MemoryHandler + Send>, ()> {
        //TODO do this in a more efficient way
        
        //Linearly search the handlers for a match
        for handler in &self.handlers {
            //Linearly search the criteria in each handler for a match
            for criterion in handler.get_match_criteria() {
                if criterion.matches(addr, access_type) {
                    return Ok(handler);
                }
            }
        }

        Err(())
    }
}

/* Functions */

//TODO
