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

//TODO (include "use" and "mod" here)

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
pub trait InstructionHandler {
    const MATCH_CRITERIA: MatchCriteria;
    
    
    //TODO
}

/* Associated Functions and Methods */

//TODO

/* Functions */

//TODO
