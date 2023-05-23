/* ffi.rs
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * FFI bindings for libirve to be used by irvers
 *
 * Based on https://rust-lang.github.io/rust-bindgen/tutorial-4.html
 *
*/

#![allow(dead_code)]
#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]

/* Imports */

include!(concat!(env!("OUT_DIR"), "/bindings.rs"));
