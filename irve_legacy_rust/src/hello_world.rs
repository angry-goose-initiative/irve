/* TODO.rs
 * Copyright (C) TODO John Jekel
 * See the LICENSE file at the root of the project for licensing info.
 *
 * TODO description
 *
*/

#![no_std]
#![no_main]

/* Imports */

//use irvervrust::panic_handler;

//TODO (include "use" and "mod" here)

/* Constants */

//TODO

/* Macros */

//TODO (also pub(crate) use the_macro statements here too)

/* Static Variables */

//TODO

/* Types */

//TODO

/* Associated Functions and Methods */

//TODO

/* Functions */

#[no_mangle]
fn main() -> ! {
    loop {}//TODO
}

//TODO why dosn't this work if it is in lib.rs?
#[panic_handler]
fn panic_handler(_info: &core::panic::PanicInfo) -> ! {
    loop {}//TODO
}

#[no_mangle]
fn __libc_init_array() {
    //TODO link with newlib properly
}

#[no_mangle]
static _edata: usize = 0;

#[no_mangle]
static _end: usize = 0;

#[no_mangle]
fn exit() -> ! {
    loop {}//TODO
}

/* Tests */

//TODO
