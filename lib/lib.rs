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

/* Static Variables */

//TODO

/* Types */

pub struct System {
    state: State,
    //TODO structure for mapping to instruction handlers
    log_sender: Option<mpsc::Sender<String>>,
}

pub struct State {
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

    fn log(self: &Self, message: String) {
        if let Some(sender) = &self.log_sender {
            sender.send(message).unwrap();
        }
    }

    pub fn get_log_receiver(self: &mut Self) -> mpsc::Receiver<String> {
        let (sender, reciever) = mpsc::channel();
        self.log_sender = Some(sender);
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
            //TODO
        }
    }
}

/* Functions */

//TODO
