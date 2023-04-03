/* NAME//TODO
 * By: John Jekel
 *
 * TODO description
 *
*/

/* Imports */

use std::sync::mpsc;
use std::thread;

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
        $system.log($level.into(), format!($($format_args),*));
    };
    ($system:expr, $($format_args:expr),*) => {
        $system.log(LogLevel::Info(0), format!($($format_args),*));
    };
}

/* Static Variables */

//TODO

/* Types */

pub enum RawInstruction {
    Regular(u32),
    Compressed(u16)
}

#[derive(Debug)]
pub enum LogLevel {
    Error,
    Warning,
    Info(u8),//Verbosity
    Debug
}

pub struct System {
    state: State,
    //TODO structure for mapping to instruction handlers
    log_sender: Option<mpsc::Sender<(LogLevel, String)>>,

    thread: Option<thread::JoinHandle<()>>//TODO return type
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
            thread: None
        };

        //system.register_instruction_handler();//TODO register base spec instruction handler
        
        system
    }

    pub fn single_step(self: &mut Self) {
        assert!(self.thread.is_none(), "Cannot single step while thread is running");
        log!(self, 1, "Executing single-step step");
        self.tick();
    }

    fn tick(self: &mut Self) {
        log!(self, 2, "Executing tick");
        let fetched_instruction = self.fetch();
        //TODO decode
        //TODO execute
        //TODO handle peripherals, interrupts, etc

    }

    fn fetch(self: &mut Self) -> RawInstruction {
        log!(self, 3, "Executing fetch");
        //TODO
        todo!();
    }

    pub fn run_in_thread(self: &mut Self) {
        assert!(self.thread.is_none(), "Cannot start running in a thread while one is already running");
        log!(self, "Starting XRVE in thread");
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
        assert!(self.log_sender.is_none(), "Cannot setup logging twice");
        let (sender, reciever) = mpsc::channel();
        self.log_sender = Some(sender);
        log!(self, "XRVE Log started");
        reciever
    }

    //Design decision: We will not allow handlers to be unregistered
    //TODO perhaps allow priorities?
    pub fn register_instruction_handler(&mut self, handler: impl instruction_handler::InstructionHandler) {
        log!(self, 1, "Registering instruction handler");
        //TODO
    }

    pub fn register_memory_handler(&mut self, handler: impl memory_handler::MemoryHandler) {
        log!(self, 1, "Registering memory handler");
        //TODO
    }

    pub fn register_csr_handler(&mut self, handler: impl csr_handler::CSRHandler) {
        log!(self, 1, "Registering CSR handler");
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
            csrs: [0; 4096],//TODO set this properly
        }
    }
}

impl From<u8> for LogLevel {
    fn from(value: u8) -> Self {
        return LogLevel::Info(value);
    }
}

/* Functions */

//TODO
