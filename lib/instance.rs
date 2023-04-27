/* instance.rs
 * By: John Jekel
 *
 * Contains the XRVE instance exposed to the user
 *
*/

/* Imports */

use crate::logging::prelude::*;

use std::sync::atomic::AtomicBool;
use std::sync::Arc;
use std::thread;
use crate::state::State;
use crate::io::IO;
use crate::instruction_handler;
use crate::pmmap::memory_handler;
use crate::csr_handler;
use crate::fetch::fetch_raw;
use crate::pmmap::PhysicalMemoryMap;
use crate::decode::Decoder;
use crate::logging::LogReciever;
use crate::logging::init_logging;

/* Constants */

//TODO

/* Macros */

//TODO (also pub(crate) use the_macro statements here too)

/* Static Variables */

//TODO

/* Types */

//TODO move this elsewhere

pub struct Instance {
    state: Option<State>,
    pmmap: Option<PhysicalMemoryMap>,
    decoder: Option<Decoder>,
    //TODO structure for mapping to instruction handlers
    l: Logger,

    io: Option<IO>,

    thread: Option<thread::JoinHandle<(State, PhysicalMemoryMap, Decoder, IO, Logger)>>,//Thread returns the state and IO when it exits
                                                   //to give us back ownership


    //We need it to be atomic to avoid tearing
    //We need an Arc so that the lifetime is static (since the thread requires static lifetimes on
    //variables it captures)
    thread_stop_request: Arc<AtomicBool>//We give the thread a reference to this so it stops when we want it to
}

/* Associated Functions and Methods */

impl Instance {
    pub fn new() -> Self {
        let system = Self {
            state: Some(State::new()),
            pmmap: Some(PhysicalMemoryMap::new()),
            decoder: Some(Decoder::new()),
            //TODO
            l: None,
            io: Some(IO::new()),
            thread: None,
            thread_stop_request: Arc::new(AtomicBool::new(false))
        };

        //system.register_instruction_handler();//TODO register base spec instruction handler
        
        system
    }

    //TODO add functions for reading and writing to memory so the user can load/dump memory

    pub fn single_step(self: &mut Self) {
        debug_assert!(self.thread.is_none(), "Cannot single step while thread is running");
        log_with!(self.l, 128, "Executing single-step step; {} instructions retired", self.state.as_ref().unwrap().retired_insts());
        tick(self.state.as_mut().unwrap(), self.pmmap.as_mut().unwrap(), self.decoder.as_mut().unwrap(), self.io.as_mut().unwrap(), &mut self.l);
    }

    pub fn run_in_thread(self: &mut Self) {
        debug_assert!(self.thread.is_none(), "Cannot start running in a thread while one is already running");
        log_with!(self.l, 0, "Starting XRVE in thread");

        //Set the thread stop request to false so that we don't exit as soon as we enter
        self.thread_stop_request.store(false, std::sync::atomic::Ordering::Relaxed);

        //Setup the variables to be moved into the closure
        let mut state = self.state.take().unwrap();
        let mut pmmap = self.pmmap.take().unwrap();
        let mut decoder = self.decoder.take().unwrap();
        let mut io = self.io.take().unwrap();
        let mut logger = self.l.take();//Recall Logger is an Option internally
        //Clone the thread stop request Arc so that we can give it to the thread
        let thread_stop_request_clone = self.thread_stop_request.clone();

        //Launch the thread and give it the state and IO
        self.thread = Some(thread::spawn(move || -> (State, PhysicalMemoryMap, Decoder, IO, Logger) {
            //We just give the actual thread function references to it dosn't have to be
            //responsible for returning them at the end
            emulation_thread(&mut state, &mut pmmap, &mut decoder, &mut io, thread_stop_request_clone, &mut logger);
            return (state, pmmap, decoder, io, logger);
        }));
    }

    pub fn stop_thread(self: &mut Self) {
        debug_assert!(self.thread.is_some(), "Cannot stop thread when one is not running");
        log_with!(self.l, 0, "Stopping XRVE thread");

        //Request that the thread stop
        self.thread_stop_request.store(true, std::sync::atomic::Ordering::Relaxed);

        //Join the thread, and take back the things we gave it
        let (state, pmmap, decoder, io, logger) = self.thread.take().unwrap().join().unwrap();
        self.state = Some(state);
        self.pmmap = Some(pmmap);
        self.decoder = Some(decoder);
        self.l = logger;
        self.io = Some(io);

        log_with!(self.l, 0, "XRVE thread stopped successfully");
    }

    pub fn get_log_receiver(self: &mut Self) -> LogReciever {
        debug_assert!(self.l.is_none(), "Cannot setup logging twice");
        debug_assert!(self.thread.is_none(), "Cannot setup logging while thread is running");

        //Initialize logging, saving the Logger in our Instance and returning the LogReciever
        let (logger, log_reciever) = init_logging();
        self.l = logger;
        log_with!(self.l, LogLevel::Info(2), "Returning log reciever to user");
        log_reciever
    }

    //Design decision: We will not allow handlers to be unregistered
    //TODO perhaps allow priorities?
    pub fn register_instruction_handler(&mut self, handler: impl instruction_handler::InstructionHandler + Send + 'static) -> Option<Box<dyn instruction_handler::InstructionHandler + Send>> {
        debug_assert!(self.thread.is_none(), "Cannot register instruction handler while thread is running");
        log_with!(self.l, 1, "Registering instruction handler");
        self.decoder.as_mut().unwrap().register_handler(handler)//Return the old handler if there was one
    }

    pub fn register_memory_handler(&mut self, handler: impl memory_handler::MemoryHandler + Send + 'static) {
        debug_assert!(self.thread.is_none(), "Cannot register memory handler while thread is running");
        log_with!(self.l, 1, "Registering memory handler");
        self.pmmap.as_mut().unwrap().register_handler(handler);
    }

    pub fn register_csr_handler(&mut self, handler: impl csr_handler::CSRHandler) {
        debug_assert!(self.thread.is_none(), "Cannot register CSR handler while thread is running");
        log_with!(self.l, 1, "Registering CSR handler");
        //todo!();
        //TODO
    }

    //pub fn add_ram

    //TODO a function for registering a handler that runs each tick?

    //Add a function for single-step execution
    //and another for creating a thread and giving it the state/list of instruction handlers
    //and another for shutting down the thread to allow for direct access again
    //

    //TODO add a function for getting a reciever for logging
    //
    //

    pub fn read_byte_from_memory(&mut self, addr: u32) -> Result<u8, ()> {
        debug_assert!(self.thread.is_none(), "Cannot read memory while thread is running");
        self.pmmap.as_mut().unwrap().read_byte(self.state.as_mut().unwrap(), addr)
    }

    pub fn write_byte_to_memory(&mut self, addr: u32, data: u8) -> Result<(), ()> {
        debug_assert!(self.thread.is_none(), "Cannot write memory while thread is running");
        self.pmmap.as_mut().unwrap().write_byte(self.state.as_mut().unwrap(), addr, data)
    }

    pub fn read_halfword_from_memory(&mut self, addr: u32) -> Result<u16, ()> {
        debug_assert!(self.thread.is_none(), "Cannot read memory while thread is running");
        self.pmmap.as_mut().unwrap().read_halfword(self.state.as_mut().unwrap(), addr)
    }

    pub fn write_halfword_to_memory(&mut self, addr: u32, data: u16) -> Result<(), ()> {
        debug_assert!(self.thread.is_none(), "Cannot write memory while thread is running");
        self.pmmap.as_mut().unwrap().write_halfword(self.state.as_mut().unwrap(), addr, data)
    }

    pub fn read_word_from_memory(&mut self, addr: u32) -> Result<u32, ()> {
        debug_assert!(self.thread.is_none(), "Cannot read memory while thread is running");
        self.pmmap.as_mut().unwrap().read_word(self.state.as_mut().unwrap(), addr)
    }

    pub fn write_word_to_memory(&mut self, addr: u32, data: u32) -> Result<(), ()> {
        debug_assert!(self.thread.is_none(), "Cannot write memory while thread is running");
        self.pmmap.as_mut().unwrap().write_word(self.state.as_mut().unwrap(), addr, data)
    }

    //TODO add functions for other access sizes?
}

/* Functions */

pub fn emulation_thread(state: &mut State, pmmap: &mut PhysicalMemoryMap, decoder: &mut Decoder, io: &mut IO, thread_stop_request: Arc<AtomicBool>, l: &mut Logger) {
    log!(0, "XRVE thread started");
    loop {
        if thread_stop_request.load(std::sync::atomic::Ordering::Relaxed) {
            log!(0, "XRVE thread stop request received");
            break;
        }

        log!(128, "Executing tick; {} instructions retired", state.retired_insts());
        tick(state, pmmap, decoder, io, l);
    }
}

pub fn tick(state: &mut State, pmmap: &mut PhysicalMemoryMap, decoder: &mut Decoder, io: &mut IO, l: &mut Logger) {
    let raw_inst = fetch_raw(state, pmmap, l);
    log!(LogLevel::Debug, "Fetched instruction: {:?}", raw_inst);//TESTING
    let inst_handle = decoder.decode(raw_inst);
    //log!(l, LogLevel::Debug, "Decoded instruction: {:?}", inst_handle);//TESTING
    let unwrapped_inst_handle = inst_handle.unwrap();//TESTING
    unwrapped_inst_handle.handle(state, pmmap, raw_inst);//, io, l);

    //TODO handle interrupts, exceptions, etc.
                                            
    //todo!();
    state.retire_inst();
}

/* Tests */

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn create_instance() {
        let _ = Instance::new();
    }

    #[test]
    fn get_log_receiver() {
        let mut instance = Instance::new();
        let _ = instance.get_log_receiver();
    }

    //TODO more tests
}
