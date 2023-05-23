/* main.rs
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Entry point for irvers
 *
*/

/* Imports */

mod irve;//We do NOT use ffi.rs directly (it is very unsafe)

/* Constants */

//TODO

/* Macros */

//TODO (also pub(crate) use the_macro statements here too)

/* Static Variables */

//TODO

/* Associated Functions and Methods */

//TODO

/* Functions */

fn main() {
    println!("libirve version {}", irve::about::get_version_string());
    todo!()
}

/* Tests */

//TODO
