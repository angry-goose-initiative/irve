/*
 * File:    build.rs
 * Brief:   Build script for libirve_disassemble
 *
 * Copyright: Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Based on cbindgen example
 *
*/

/*!
 * TODO rustdoc for this file here
*/

/* ------------------------------------------------------------------------------------------------
 * Uses
 * --------------------------------------------------------------------------------------------- */

extern crate cbindgen;

/* ------------------------------------------------------------------------------------------------
 * Functions
 * --------------------------------------------------------------------------------------------- */

fn main() {
    let crate_directory_path = std::env::var("CARGO_MANIFEST_DIR").unwrap();
    let crate_directory_path_ref = &crate_directory_path;

    println!("cargo:rerun-if-changed={crate_directory_path_ref}/../../build/rust/target/irve_disassemble.h");

    cbindgen::Builder::new()
        .with_crate(crate_directory_path_ref)
        .with_language(cbindgen::Language::Cxx)
        .with_include_guard("IRVE_DISASSEMBLE_H")
        .with_namespaces(&["irve", "internal", "disassemble"])
        .with_tab_width(4)
        .generate()
        .expect("cbindgen should sucessfully generate bindings")
        .write_to_file(format!("{crate_directory_path_ref}/../../build/rust/target/irve_disassemble.h"));
}

/* ------------------------------------------------------------------------------------------------
 * Tests
 * --------------------------------------------------------------------------------------------- */

#[cfg(test)]
mod tests {
    use super::*;

    //#[test]
    //TODO
}
