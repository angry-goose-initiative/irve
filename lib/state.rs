/* state.rs
 * By: John Jekel
 *
 * Contains the state of the RISC-V system
 *
*/

/* Imports */

//TODO (include "use" and "mod" here)

/* Constants */

//TODO

/* Macros */

//TODO (also pub(crate) use the_macro statements here too)

/* Static Variables */

//TODO

/* Types */

pub struct State {
    pc: u32,
    registers: [u32; 31],

    physical_memory: Box<[u8]>,
    insts_retired: u64,

    boot_time: std::time::Instant//Used for calculating time since boot for one of the RISC-V CSRs

    //TODO
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
    }
}

/* Functions */

//TODO
