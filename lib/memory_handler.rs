/* NAME//TODO
 * By: John Jekel
 *
 * TODO description
 *
 * Like instruction handler, but for PHYSICAL memory addresses
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
    SingleAddress(u32),
    AddressRange(u32, u32),//Inclusive

    Never,

    //TODO
}

//TODO may need a lifetime parameter here for the callback
//IMPORTANT: Actually, no, the InstructionHandler will be consumed when it is registered
//and if the user wants to communicate with their own code, they can use a channel
pub trait MemoryHandler /*<const NUM_CRITERIA: usize>*/ {
    const MATCH_CRITERIA: MatchCriteria;//[MatchCriteria; NUM_CRITERIA];
   
    fn handle(&self, state: &mut State/*, addr: address*/)/* -> Result<(), Error>*/;
    
    //TODO
}

/* Associated Functions and Methods */

impl MatchCriteria {
    pub fn matches(&self, addr: u32) -> bool {
        match self {
            MatchCriteria::Always               => true,
            MatchCriteria::SingleAddress(a)     => addr == *a,
            MatchCriteria::AddressRange(a, b)   => (addr >= *a) && (addr <= *b),
            MatchCriteria::Never                => false,
        }
    }
}

/* Functions */

//TODO
