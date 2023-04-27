/* NAME//TODO
 * By: John Jekel
 *
 * TODO description
 *
 * Like instruction handler, but for csr addresses
 *
*/

/* Imports */

use crate::state::State;

/* Constants */

//TODO

/* Macros */

//TODO (also pub(crate) use the_macro statements here too)

/* Static Variables */

//TODO

/* Types */

//pub struct InstructionHandler {
    //TODO
//}

pub enum MatchCriteria {
    Always,

    Never,

    //TODO
}

//TODO may need a lifetime parameter here for the callback
//IMPORTANT: Actually, no, the InstructionHandler will be consumed when it is registered
//and if the user wants to communicate with their own code, they can use a channel
pub trait CSRHandler {
    const MATCH_CRITERIA: MatchCriteria;
   
    fn handle(&self, state: &mut State/*, addr: address*/)/* -> Result<(), Error>*/;
    
    //TODO
}

/* Associated Functions and Methods */

//TODO

/* Functions */

//TODO
