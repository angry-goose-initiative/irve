/*
 * File:    ffi.rs
 * Brief:   FFI bindings for libirve to be used by irvers
 *
 * Copyright: Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * TODO longer description
 *
 * Based on https://rust-lang.github.io/rust-bindgen/tutorial-4.html
 *
*/

/*!
 * TODO rustdoc for this file here
*/

#![allow(dead_code)]
#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]

/* ------------------------------------------------------------------------------------------------
 * Uses
 * --------------------------------------------------------------------------------------------- */

include!(concat!(env!("OUT_DIR"), "/bindings.rs"));

/* ------------------------------------------------------------------------------------------------
 * Tests
 * --------------------------------------------------------------------------------------------- */

#[cfg(test)]
mod tests {
    use super::*;

    //#[test]
    //TODO
}
