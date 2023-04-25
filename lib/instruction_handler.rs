/* NAME//TODO
 * By: John Jekel
 *
 * TODO description
 *
 * TODO this file will be a pair that takes the state and the decoded opcode and performs an
 * operation if it matches; the pair will be registered within the state/system and be called to
 * handle the opcode if it matches
 *
*/

/* Imports */

use crate::state::State;
use crate::decode::MajorOpcode;
use crate::pmmap::PhysicalMemoryMap;
use crate::fetch::RawInstruction;

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


/*pub enum MatchCriteria {
    Always,
    Opcode(MajorOpcode),
    Never,

    //TODO
}*/

//TODO may need a lifetime parameter here for the callback
//IMPORTANT: Actually, no, the InstructionHandler will be consumed when it is registered
//and if the user wants to communicate with their own code, they can use a channel
pub trait InstructionHandler {
    //const MATCH_CRITERIA: MatchCriteria;
   
    //fn handle(&self, state: &mut State/*, opcode: &Opcode*/)/* -> Result<(), Error>*/;
    fn get_major_opcode_handled(&self) -> MajorOpcode;
    fn handle(&self, state: &mut State, pmmap: &mut PhysicalMemoryMap, instruction: RawInstruction);
    
    //TODO
}

/* Associated Functions and Methods */

//TODO
//

/*impl MatchCriteria {
    pub fn satisfies(&self, opcode: MajorOpcode) -> bool {
        match self {
            MatchCriteria::Always => true,
            MatchCriteria::Opcode(match_opcode) => opcode == *match_opcode,
            MatchCriteria::Never => false,
        }
    }
}*/


/* Functions */

//TODO

/* Tests */

//TODO
