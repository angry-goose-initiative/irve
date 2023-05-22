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

#[derive(Debug)]
#[repr(C)]
pub enum Format {
    //NOTE: It is imperative these numbers match those in irve::internal::decode::inst_format_t
    //over on the C++ side of things
    R = 0,
    I = 1,
    S = 2,
    B = 3,
    U = 4,
    J = 5
}

#[derive(Debug)]
#[repr(C)]
pub enum Opcode {
    //NOTE: It is imperative these numbers match those in irve::internal::decode::opcode_t
    //over on the C++ side of things
    //TODO better formatting of this
    Load = 0b00000,     LoadFp = 0b00001,  Custom0 = 0b00010,     MiscMem = 0b00011, OpImm = 0b00100,   AuiPc= 0b00101,        OpImm32 = 0b00110,    B480 = 0b00111,
    Store = 0b01000,    StoreFp = 0b01001, Custom1 = 0b01010,     Amo = 0b01011,      Op = 0b01100,       Lui = 0b01101,          Op32 = 0b01110,        B64 = 0b01111,
    MAdd = 0b10000,     MSub = 0b10001,     NMSub = 0b10010,        NMAdd = 0b10011,    OpFp = 0b10100,    Reserved0 = 0b10101,   Custom2 = 0b10110,     B481 = 0b10111,
    Branch = 0b11000,   Jalr = 0b11001,     Reserved1 = 0b11010,   Jal = 0b11011,      System = 0b11100,   Reserved3 = 0b11101,   Custom3 = 0b11110,     BGE80 = 0b11111
}

#[derive(Debug)]
#[repr(C)]
pub struct RawInst {
    format: Format,
    opcode: Opcode,
    rd: u8,
    rs1: u8,
    rs2: u8,
    funct3: u8,
    funct5: u8,
    funct7: u8,
    imm: u32
}

/* Associated Functions and Methods */

impl RawInst {
    //The Main Disassembly Function
    fn disassemble(&self) -> String {//TODO implement this function
        //return "TODO: irve_disassemble is not fully implemented yet!".to_string();
        return format!("TODO: {:?}", self);
        //todo!()
    }
}

/* FFI-Related Functions */

#[no_mangle]
pub extern "C" fn disassemble(raw_inst: &RawInst) -> *mut std::os::raw::c_char {
    let disassembly_string = raw_inst.disassemble();
    let owned_c_string = std::ffi::CString::new(disassembly_string)
        .expect("The disassembly shouldn't contain any null bytes (it should be human-readable)!");
    let raw_c_string = owned_c_string.into_raw();
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
