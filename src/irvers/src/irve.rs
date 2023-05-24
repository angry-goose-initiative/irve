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

pub mod logging {
    use super::ffi;

    //Thanks https://stackoverflow.com/questions/71985357/whats-the-best-way-to-write-a-custom-format-macro

    macro_rules! irvelog {
        ($indent:expr, $($format_args:tt)*) => {{
            irve::logging::log($indent, &format!($($format_args)*));
        }};
    }
    pub(crate) use irvelog;
    macro_rules! irvelog_always {
        ($indent:expr, $($format_args:tt)*) => {{
            irve::logging::log_always($indent, &format!($($format_args)*));
        }};
    }
    pub(crate) use irvelog_always;

    pub fn log(indent: u8, text: &str) {
        let owned_text_c_str = std::ffi::CString::new(text).expect("Text provided to irve::logging::log() should not contain null characters");
        let text_raw_str = owned_text_c_str.into_raw();
        unsafe { ffi::irve_logging_log(indent, text_raw_str) };
    }

    pub fn log_always(indent: u8, text: &str) {
        let owned_text_c_str = std::ffi::CString::new(text).expect("Text provided to irve::logging::log() should not contain null characters");
        let text_raw_str = owned_text_c_str.into_raw();
        unsafe { ffi::irve_logging_log_always(indent, text_raw_str) };
    }

    //TODO others
}

pub mod about {
    use super::ffi;

    //TODO comments for why what we're doing is safe

    

    macro_rules! get_safe_str_from {
        ($unsafe_irve_function:ident) => {{
            let raw_str = unsafe { ffi::$unsafe_irve_function() };
            let c_str = unsafe { std::ffi::CStr::from_ptr(raw_str) };
            let str = c_str.to_str().expect("IRVE strings should only contain valid UTF-8 characters");
            str
        }};
    }

    pub fn get_version_major() -> u128 {
        unsafe { ffi::irve_about_get_version_major() }.into()
    }

    pub fn get_version_minor() -> u128 {
        unsafe { ffi::irve_about_get_version_minor() }.into()
    }

    pub fn get_version_patch() -> u128 {
        unsafe { ffi::irve_about_get_version_patch() }.into()
    }

    pub fn get_version_string() -> &'static str {
        get_safe_str_from!(irve_about_get_version_string)
    }

    pub fn get_build_time_string() -> &'static str {
        get_safe_str_from!(irve_about_get_build_time_string)
    }

    pub fn get_build_date_string() -> &'static str {
        get_safe_str_from!(irve_about_get_build_date_string)
    }

    pub fn get_build_system_string() -> &'static str {
        get_safe_str_from!(irve_about_get_build_system_string)
    }

    pub fn get_build_host_string() -> &'static str {
        get_safe_str_from!(irve_about_get_build_host_string)
    }

    pub fn get_compile_target_string() -> &'static str {
        get_safe_str_from!(irve_about_get_compile_target_string)
    }

    pub fn get_compiler_string() -> &'static str {
        get_safe_str_from!(irve_about_get_compiler_string)
    }
}

/* Functions */

//TODO
