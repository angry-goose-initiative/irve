/* lib.rs
 * By: John Jekel
 *
 * The eXtensible RISC-V Emulator library
 *
*/

/* Imports */

use std::sync::atomic::AtomicBool;
use std::sync::mpsc;
use std::sync::Arc;
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
    state: Option<State>,
    //TODO structure for mapping to instruction handlers
    log_sender: Option<mpsc::Sender<(LogLevel, String)>>,//TODO move this to IO

    io: Option<IO>,

    thread: Option<thread::JoinHandle<(State, IO)>>,//Thread returns the state and IO when it exits
                                                   //to give us back ownership

    thread_stop_request: Arc<AtomicBool>//We give the thread a reference to this so it stops when we want it to
}

pub struct IO {
    log_sender: Option<mpsc::Sender<(LogLevel, String)>>,
    //TODO
}

pub struct State {
    pc: u32,
    registers: [u32; 31],

    physical_memory: Box<[u8]>,
    insts_retired: u64,

    boot_time: std::time::Instant//Used for calculating time since boot for one of the RISC-V CSRs

    //TODO
}

/* Associated Functions and Methods */

impl System {
    pub fn new() -> Self {
        let mut system = Self {
            state: Some(State::new()),
            //TODO
            log_sender: None,
            io: Some(IO::new()),
            thread: None,
            thread_stop_request: Arc::new(AtomicBool::new(false))
        };

        //system.register_instruction_handler();//TODO register base spec instruction handler
        
        system
    }

    pub fn single_step(self: &mut Self) {
        assert!(self.thread.is_none(), "Cannot single step while thread is running");
        log!(self, 1, "Executing single-step step");
        self.tick();
    }

    //TODO move this to state?
    fn tick(self: &mut Self) {
        log!(self, 2, "Executing tick");
        let fetched_instruction = self.fetch();
        //TODO decode
        //TODO execute
        //TODO handle peripherals, interrupts, etc

        //self.state.insts_retired += 1;
    }

    fn fetch(self: &mut Self) -> RawInstruction {
        log!(self, 3, "Executing fetch");
        //TODO
        todo!();
    }

    pub fn run_in_thread(self: &mut Self) {
        assert!(self.thread.is_none(), "Cannot start running in a thread while one is already running");
        log!(self, "Starting XRVE in thread");

        //Set the thread stop request to false so that we don't exit as soon as we enter
        self.thread_stop_request.store(false, std::sync::atomic::Ordering::Relaxed);

        //Setup the variables to be moved into the closure
        let state = self.state.take().unwrap();
        let io = self.io.take().unwrap();
        //Clone the thread stop request Arc so that we can give it to the thread
        let thread_stop_request_clone = self.thread_stop_request.clone();

        //Launch the thread and give it the state and IO
        self.thread = Some(thread::spawn(move || -> (State, IO) {
            return Self::the_thread(state, io, thread_stop_request_clone);
        }));
    }

    pub fn stop_thread(self: &mut Self) {
        assert!(self.thread.is_some(), "Cannot stop thread when one is not running");
        log!(self, "Stopping XRVE thread");

        //Request that the thread stop, take and join the thread handle, and take back the state and IO
        self.thread_stop_request.store(true, std::sync::atomic::Ordering::Relaxed);
        let thread = self.thread.take().unwrap();
        let (state, io) = thread.join().unwrap();
        self.state = Some(state);
        self.io = Some(io);

        log!(self, "XRVE thread stopped successfully");
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
    //

    //TODO move this into a separate file, perhaps not even a member function, but just a free
    //function that takes a state and IO and returns a state and IO
    pub fn the_thread(state: State, io: IO, thread_stop_request: Arc<AtomicBool>) -> (State, IO) {
        //TODO
        todo!();
        return (state, io);
    }
}

impl IO {
    pub fn new() -> Self {
        Self {
            log_sender: None,
            //TODO
        }
    }
}

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

impl From<u8> for LogLevel {
    fn from(value: u8) -> Self {
        return LogLevel::Info(value);
    }
}

/* Functions */

//TODO
