/* NAME//TODO
 * By: John Jekel
 *
 * TODO description
 *
*/

/* Imports */

use std::sync::mpsc;

mod instruction_handler;
mod memory_handler;
mod csr_handler;
mod decode;
//TODO (include "use" and "mod" here)

/* Constants */

//TODO

/* Macros */

//TODO (also pub(crate) use the_macro statements here too)
macro_rules! log {
    ($system:expr, $level:expr, $($format_args:expr),*) => {
        $system.log($level, format!($($format_args),*));
    };
    ($system:expr, $($format_args:expr),*) => {
        $system.log(LogLevel::NormalInfo, format!($($format_args),*));
    };
}

/* Static Variables */

//TODO

/* Types */

#[derive(Debug)]
pub enum LogLevel {
    Error,
    Warning,
    NormalInfo,
    VerboseInfo{verbosity: u128},
    Debug
}

pub struct System {
    state: State,
    //TODO structure for mapping to instruction handlers
    log_sender: Option<mpsc::Sender<(LogLevel, String)>>,
}

pub struct State {
    pc: u32,
    registers: [u32; 31],

    physical_memory: Box<[u8]>,
    csrs: [u32; 4096],

    //TODO
}

/* Associated Functions and Methods */

impl System {
    pub fn new() -> Self {
        let mut system = Self {
            state: State::new(),
            //TODO
            log_sender: None,
        };

        //system.register_instruction_handler();//TODO register base spec instruction handler
        
        system
    }

    pub fn single_step(self: &mut Self) {
        log!(self, LogLevel::VerboseInfo{verbosity: 1}, "Executing step");
        //TODO
    }

    pub fn run_in_thread(self: &mut Self) {
        log!(self, LogLevel::Info, "Starting XRVE in thread");
        //log!(self, LogLevel::Debug, "Testing macro with arguments {}", 123);
        //log!(self, "Testing macro without log level"); 
        //TODO
    }

    fn log(self: &Self, level: LogLevel, message: String) {
        if let Some(sender) = &self.log_sender {
            sender.send((level, message)).unwrap();
        }
    }

    pub fn get_log_receiver(self: &mut Self) -> mpsc::Receiver<(LogLevel, String)> {
        let (sender, reciever) = mpsc::channel();
        self.log_sender = Some(sender);
        log!(self, "XRVE Log started");
        reciever
    }

    //Design decision: We will not allow handlers to be unregistered
    //TODO perhaps allow priorities?
    pub fn register_instruction_handler(&mut self, handler: impl instruction_handler::InstructionHandler) {
        //TODO
    }

    pub fn register_memory_handler(&mut self, handler: impl memory_handler::MemoryHandler) {
        //TODO
    }

    pub fn register_csr_handler(&mut self, handler: impl csr_handler::CSRHandler) {
        //TODO
    }

    //TODO a function for registering a handler that runs each tick?

    //Add a function for single-step execution
    //and another for creating a thread and giving it the state/list of instruction handlers
    //and another for shutting down the thread to allow for direct access again
    //

    //TODO add a function for getting a reciever for logging
}

impl State {
    pub fn new() -> Self {
        Self {
            pc: 0,
            registers: [0; 31],
            physical_memory: vec![0; 0x1000].into_boxed_slice(),//TODO set this properly
            //TODO
        }
    }
}

/* Functions */

//TODO
