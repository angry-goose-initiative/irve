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
macro_rules! log {//TODO have the log accept an Option<Logger> and use that instead of the system
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

//TODO figure out a clean way to do this and still have consistent macros
type Logger = mpsc::Sender<(LogLevel, String)>;//TODO change this to an Option<mpsc::Sender<(LogLevel, String)>>

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

pub struct System {//TODO rename to Instance
    state: Option<State>,
    //TODO structure for mapping to instruction handlers
    logger: Option<Logger>,

    io: Option<IO>,

    thread: Option<thread::JoinHandle<(State, Option<Logger>, IO)>>,//Thread returns the state and IO when it exits
                                                   //to give us back ownership

    //We need it to be atomic to avoid tearing
    //We need an Arc so that the lifetime is static (since the thread requires static lifetimes on
    //variables it captures)
    thread_stop_request: Arc<AtomicBool>//We give the thread a reference to this so it stops when we want it to
}

pub struct IO {
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

impl System {//TODO rename to Instance
    pub fn new() -> Self {
        let mut system = Self {
            state: Some(State::new()),
            //TODO
            logger: None,
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
        let mut state = self.state.take().unwrap();
        let mut logger = self.logger.take();
        let mut io = self.io.take().unwrap();
        //Clone the thread stop request Arc so that we can give it to the thread
        let thread_stop_request_clone = self.thread_stop_request.clone();

        //Launch the thread and give it the state and IO
        self.thread = Some(thread::spawn(move || -> (State, Option<Logger>, IO) {
            Self::the_thread(&mut state, &mut logger, &mut io, thread_stop_request_clone);
            return (state, logger, io);
        }));
    }

    pub fn stop_thread(self: &mut Self) {
        assert!(self.thread.is_some(), "Cannot stop thread when one is not running");
        log!(self, "Stopping XRVE thread");

        //Request that the thread stop, take and join the thread handle, and take back the state and IO
        self.thread_stop_request.store(true, std::sync::atomic::Ordering::Relaxed);
        let thread = self.thread.take().unwrap();
        let (state, logger, io) = thread.join().unwrap();
        self.state = Some(state);
        self.logger = logger;
        self.io = Some(io);

        log!(self, "XRVE thread stopped successfully");
    }

    fn log(self: &Self, level: LogLevel, message: String) {
        if let Some(logger) = &self.logger {
            logger.send((level, message)).unwrap();
        }
    }

    pub fn get_log_receiver(self: &mut Self) -> mpsc::Receiver<(LogLevel, String)> {
        assert!(self.logger.is_none(), "Cannot setup logging twice");
        //TODO also ensure that the thread is not running
        let (sender, reciever) = mpsc::channel();
        self.logger = Some(sender);
        log!(self, "XRVE Log started");
        reciever
    }

    //Design decision: We will not allow handlers to be unregistered
    //TODO perhaps allow priorities?
    pub fn register_instruction_handler(&mut self, handler: impl instruction_handler::InstructionHandler) {
        log!(self, 1, "Registering instruction handler");
        todo!();
        //TODO
    }

    pub fn register_memory_handler(&mut self, handler: impl memory_handler::MemoryHandler) {
        log!(self, 1, "Registering memory handler");
        todo!();
        //TODO
    }

    pub fn register_csr_handler(&mut self, handler: impl csr_handler::CSRHandler) {
        log!(self, 1, "Registering CSR handler");
        todo!();
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
    pub fn the_thread(state: &mut State, logger: &mut Option<Logger>, io: &mut IO, thread_stop_request: Arc<AtomicBool>) {
        //log!(logger, 1, "XRVE thread started");
        loop {
            if thread_stop_request.load(std::sync::atomic::Ordering::Relaxed) {
                //log!(logger, 1, "XRVE thread stop request received");
                break;
            }

            //TODO
            todo!();
        }
    }
}

impl IO {
    pub fn new() -> Self {
        Self {
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
