/* about.rs
 * By: John Jekel
 *
 * Information about the eXtensible RISC-V Emulator's library
 *
*/

//!Holds information about the libxrve library for compatibilty, versioning, and licensing

pub const CARGO: &str = env!("CARGO");

pub const MANIFEST_DIR: &str = env!("CARGO_MANIFEST_DIR");

pub mod version {
    //use xrve_proc_macro::unstringify;

    ///str of libxrve's semantic version
    pub const STRING: &str =    env!("CARGO_PKG_VERSION");

    ///libxrve's semantic version, major number
    pub const MAJOR: u128 =     const_parse_to_u128(env!("CARGO_PKG_VERSION_MAJOR"));
    //pub const MAJOR: u128 =     unstringify!(env!("CARGO_PKG_VERSION_MAJOR"));

    ///libxrve's semantic version, minor number
    pub const MINOR: u128 =     const_parse_to_u128(env!("CARGO_PKG_VERSION_MINOR"));

    ///libxrve's semantic version, patch number
    pub const PATCH: u128 =     const_parse_to_u128(env!("CARGO_PKG_VERSION_PATCH"));

    //Workaround since parse isn't const yet
    const fn const_parse_to_u128(string: &str) -> u128 {
        //for loops not allowed in const fns yet (also not chars())
        /*for c in s.chars() {
            result *= 10;
            result += (c.to_digit(10).unwrap() as u128);
        }*/

        //Workaround: use as_bytes() and manually iterate through the string
        let string_bytes = string.as_bytes();
        debug_assert!(string_bytes.len() > 0, "Cannot parse empty string to u128");
        let mut result: u128 = 0;

        //Most significant digit -> least significant digit
        let mut i = 0;
        while i < string_bytes.len() {
            //Shift up existing digits by one decimal place
            result *= 10;

            //Get the next digit and add it to result
            let u8_character = string_bytes[i];
            debug_assert!(u8_character >= ('0' as u8) && u8_character <= ('9' as u8), "Cannot parse non-numerical string to u128");
            let digit = u8_character - ('0' as u8);
            result += digit as u128;

            i += 1;
        }
        result
    }

    #[cfg(test)]
    mod tests {
        use super::*;

        #[test]
        fn const_parse_u128_works() {
            debug_assert_eq!(const_parse_to_u128("0"), 0);
            debug_assert_eq!(const_parse_to_u128("123"), 123);
            debug_assert_eq!(const_parse_to_u128("1234567890"), 1234567890);
            debug_assert_eq!(const_parse_to_u128("123456789012345678901234567890"), 123456789012345678901234567890);
            debug_assert_eq!(const_parse_to_u128("0000000000"), 0);
            debug_assert_eq!(const_parse_to_u128("9876543210"), 9876543210);
        }
    }
}

pub const AUTHORS: &str =           env!("CARGO_PKG_AUTHORS");

pub const NAME: &str =              env!("CARGO_PKG_NAME");

pub const DESCRIPTION: &str =       env!("CARGO_PKG_DESCRIPTION");

pub const HOMEPAGE: &str =          env!("CARGO_PKG_HOMEPAGE");

pub const REPOSITORY: &str =        env!("CARGO_PKG_REPOSITORY");

pub const LICENSE: &str =           env!("CARGO_PKG_LICENSE");

pub const LICENSE_FILE: &str =      env!("CARGO_PKG_LICENSE_FILE");

pub const MIN_RUST_VERSION: &str =  env!("CARGO_PKG_RUST_VERSION");

pub const CRATE_NAME: &str =        env!("CARGO_CRATE_NAME");

///License text for libxrve
pub const LICENSE_TEXT: &str =  "
MIT License

Copyright (c) 2023 John Jekel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the \"Software\"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
";
