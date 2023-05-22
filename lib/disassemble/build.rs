/* build.rs
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Build script for libirve_disassemble
 *
 * Based on cbindgen example
*/

/* Imports */

extern crate cbindgen;

/* Function Implementations */

fn main() {
    println!("cargo:rerun-if-changed=lib/target/irve_disassemble.h");

    let crate_directory_path = std::env::var("CARGO_MANIFEST_DIR").unwrap();

    cbindgen::Builder::new()
        .with_crate(crate_directory_path)
        .with_language(cbindgen::Language::Cxx)
        .with_include_guard("IRVE_DISASSEMBLE_H")
        .with_namespaces(&["irve", "internal", "disassemble"])
        .with_tab_width(4)
        .generate()
        .expect("cbindgen should sucessfully generate bindings")
        .write_to_file("target/irve_disassemble.h");
}

/* Tests */

#[cfg(test)]
mod tests {
    use super::*;

    //#[test]
    //TODO
}
