/* irve.rs
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Abstractions over ffi.rs to do things in a more Rusty way
 *
*/

//TODO move this to lib/lib.rs

/* Imports */

mod ffi;

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
pub mod about {
    use super::ffi;

    pub fn get_version_string() -> &'static str {
        let raw_version_string = unsafe { ffi::irve_about_get_version_string() };
        let version_c_str = unsafe { std::ffi::CStr::from_ptr(raw_version_string) };
        let version_str = version_c_str.to_str().expect("IRVE's version string should only contain valid UTF-8 characters");
        version_str
    }
}

/* Functions */

//TODO
