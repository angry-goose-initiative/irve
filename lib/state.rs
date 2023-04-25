/* state.rs
 * By: John Jekel
 *
 * Contains the state of the RISC-V system
 * //TODO have this hold everthing but the logger, and then create a new thing called CPUState that just holds registers and stuff
 *
*/

/* Imports */

//TODO

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

    //physical_memory: Box<[u8]>,
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
            //TODO
            insts_retired: 0,
            boot_time: std::time::Instant::now()
            //TODO interrupts, CSR registers MAYBE (could just be in the handlers like ram)
        }

        //TODO register default handlers here
    }

    pub fn pc(&self) -> u32 {
        self.pc
    }

    pub fn get_r(&self, r: u8) -> u32 {
        debug_assert!(r < 32);
        if r == 0 {
            0
        } else {
            self.registers[(r as usize) - 1]
        }
    }

    pub fn set_r(&mut self, r: u8, val: u32) {
        debug_assert!(r < 32);
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

    pub fn time_since_boot(&self) -> std::time::Duration {
        self.boot_time.elapsed()
    }
}

/* Functions */

//TODO

/* Tests */

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn create_state() {
        let _ = State::new();
    }

    #[test]
    fn retires_correctly() {
        let mut state = State::new();

        for i in 0..100 {
            assert_eq!(state.retired_insts(), i);
            state.retire_inst();
        }
    }

    #[test]
    fn gp_registers_working() {
        let mut state = State::new();

        for i in 0..32 {
            state.set_r(i, (i as u32) * 123 + 456);
        }

        assert_eq!(state.get_r(0), 0);
        for i in 1..32 {
            assert_eq!(state.get_r(i), (i as u32) * 123 + 456);
        }
    }

    //TODO more tests
}
