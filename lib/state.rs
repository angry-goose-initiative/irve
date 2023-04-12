/* state.rs
 * By: John Jekel
 *
 * Contains the state of the RISC-V system
 *
*/

/* Imports */

//TODO (include "use" and "mod" here)
use crate::instruction_handler;
use crate::csr_handler;

use crate::pmmap;
use pmmap::memory_handler;

/* Constants */

//TODO

/* Macros */

//TODO (also pub(crate) use the_macro statements here too)

/* Static Variables */

//TODO

/* Types */

pub struct State {
    //TODO group registers into a separate struct
    pc: u32,
    registers: [u32; 31],

    physical_memory: Box<[u8]>,
    insts_retired: u64,

    boot_time: std::time::Instant//Used for calculating time since boot for one of the RISC-V CSRs

    //TODO add some sort of handler state
    
    //TODO have the state maintain a logger too for handlers to use
}

/* Associated Functions and Methods */

impl State {
    pub fn new() -> Self {
        Self {
            pc: 0,
            registers: [0; 31],
            physical_memory: vec![0; 0x1000].into_boxed_slice(),//TODO set this properly
            //TODO
            insts_retired: 0,
            boot_time: std::time::Instant::now()
            
        }

        //TODO register default handlers here
    }

    pub fn pc(&self) -> u32 {
        self.pc
    }

    pub fn get_r(&self, r: u8) -> u32 {
        assert!(r < 32);
        if r == 0 {
            0
        } else {
            self.registers[(r as usize) - 1]
        }
    }

    pub fn set_r(&mut self, r: u8, val: u32) {
        assert!(r < 32);
        if r != 0 {
            self.registers[(r as usize) - 1] = val;
        }
    }

    pub fn retire_inst(&mut self) {
        //Use wrapping_add to prevent overflow
        self.insts_retired = self.insts_retired.wrapping_add(1);
    }

    pub fn retired_insts(&self) -> u64 {
        self.insts_retired
    }

    //Design decision: We will not allow handlers to be unregistered
    //TODO perhaps allow priorities?
    //TODO perhaps have handlers be in a seperate struct than state to keep state small?
    pub fn register_instruction_handler(&mut self, handler: impl instruction_handler::InstructionHandler) {
        todo!();
        //TODO
    }

    pub fn register_memory_handler(&mut self, handler: impl memory_handler::MemoryHandler) {
        todo!();
        //TODO
    }

    pub fn register_csr_handler(&mut self, handler: impl csr_handler::CSRHandler) {
        todo!();
        //TODO
    }
}

/* Functions */

//TODO
