/* NAME//TODO
 * By: John Jekel
 *
 * TODO description
 *
*/

/* Imports */

mod instruction_handler;
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
        };

        //system.register_instruction_handler();//TODO register base spec instruction handler
        
        system
    }

    //Design decision: We will not allow handlers to be unregistered
    //TODO perhaps allow priorities?
    pub fn register_instruction_handler(&mut self, handler: impl instruction_handler::InstructionHandler) {
        //TODO
    }

    pub fn register_memory_handler(&mut self, handler: impl memory_handler::MemoryHandler) {
        //TODO
    }



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
