/* disassemble.rs
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * The module that does the actual disassembly
 *
*/

/* Imports */

use crate::ffi::DecodedInst;

/* Constants */

//TODO

/* Macros */

//TODO (also pub(crate) use the_macro statements here too)

/* Static Variables */

//TODO

/* Associated Functions and Methods */

impl DecodedInst {
    //The main disassembly function
    pub fn disassemble(&self) -> String {//TODO implement this function
        //return "TODO: irve_disassemble is not fully implemented yet!".to_string();
        return format!("TODO: {:?}", self);
        //todo!()
    }
}

/* Functions */

//TODO

/* Tests */

#[cfg(test)]
mod tests {
    use super::*;

    //#[test]
    //TODO
}
