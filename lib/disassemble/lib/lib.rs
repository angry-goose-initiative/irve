/* lib.rs
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Root module for irve_disassemble
 *
*/

/* Imports */

//TODO

/* Constants */

//TODO

/* Macros */

//TODO (also pub(crate) use the_macro statements here too)

/* Static Variables */

//TODO

/* Types */

/*struct RawInst {
    opcode: u8,
    rd: u8,
    rs1: u8,
    rs2: u8,
    funct3: u8,
    funct7: u8,
    imm: u32,
}
*/

/* Associated Functions and Methods */

//TODO

/* The Main Disassembly Function */

//pub fn actual_disassemble(raw_inst: u32) -> String {
pub fn actual_disassemble(_: u32) -> String {//TODO implement this function
    return "TODO: irve_disassemble is not fully implemented yet!".to_string();
    //todo!()
}

/* FFI-Related Functions */

#[no_mangle]
pub extern "C" fn disassemble(raw_inst: u32) -> *const std::os::raw::c_char {
    let disassembly_string = actual_disassemble(raw_inst);
    let owned_c_string = std::ffi::CString::new(disassembly_string)
        .expect("The disassembly shouldn't contain any null bytes (it should be human-readable)!");
    let raw_c_string: *const std::os::raw::c_char = owned_c_string.into_raw();
    raw_c_string
}

#[no_mangle]
pub extern "C" fn free_disassembly(disassembly: *mut std::os::raw::c_char) {
    if disassembly.is_null() {
        panic!("Attempted to free a null pointer in irve::internal::disassemble::free_disassembly()!");
    } else {
        //Safety is contingent on this being a pointer to a string originally returned by disassemble()
        //We can't really verify this beyond the null check above, so we'll just have to trust the C++ user
        unsafe {
            //This will take ownership of the pointer again and free it when it is dropped
            drop(std::ffi::CString::from_raw(disassembly));
        }
    }
}

/* Tests */

#[cfg(test)]
mod tests {
    use super::*;

    //#[test]
    //TODO
}
