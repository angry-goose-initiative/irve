/* NAME//TODO
 * By: John Jekel
 *
 * TODO description
 *
*/

/* Imports */

//TODO (include "use" and "mod" here)
use crate::state::State;
crate::logging::use_logging!();

/* Constants */

//TODO

/* Macros */

//TODO (also pub(crate) use the_macro statements here too)

/* Static Variables */

//TODO

/* Types */

//TODO
pub enum RawInstruction {
    Regular(u32),
    Compressed(u16),
    Unaligned,
    Fault,
}

/* Associated Functions and Methods */

//TODO

/* Functions */

pub fn fetch_raw(state: &mut State, logger: &mut Logger) -> RawInstruction {
    log!(logger, 129, "Fetching instruction from address 0x{:08x}", state.pc());

    if (state.pc() % 2) != 0 {
        log!(logger, 130, "Address isn't aligned to a 2-byte boundary");
        return RawInstruction::Unaligned;
    }

    //TODO
    //todo!();
    return RawInstruction::Regular(0);//TESTING
}

