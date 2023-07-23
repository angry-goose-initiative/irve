/*
 * File:    build.rs
 * Brief:   Build script for irvers
 *
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * TODO longer description
 * 
 * Based on cbindgen and bindgen examples
 *
*/

/*!
 * TODO rustdoc for this file here
*/

/* ------------------------------------------------------------------------------------------------
 * Uses
 * --------------------------------------------------------------------------------------------- */

extern crate bindgen;

/* ------------------------------------------------------------------------------------------------
 * Functions
 * --------------------------------------------------------------------------------------------- */

fn main() {
    println!("cargo:rustc-link-lib=static=irve");
    
    //FIXME workaround since we can't tell cargo where the irve_disassembly static library should go
    match std::env::var("PROFILE").unwrap().as_str() {
        "debug"   => { println!("cargo:rustc-link-lib=static=irve_disassemble"); }
        "release" => { println!("cargo:rustc-link-lib=static=irve_disassemble_release"); }
        _         => { panic!("Unknown build profile"); }
    }

    println!("cargo:rustc-link-lib=stdc++");//TODO will this break if the system only has Clang's libc++ and not GNU's libstdc++?
    println!("cargo:rustc-link-search=./");

    println!("cargo:rerun-if-changed=wrapper.h");
    println!("cargo:rerun-if-changed=../../include/irve_public_api.h");
    //TODO also need to rerun if anything in project root/lib changes

    let output_path = std::path::PathBuf::from(std::env::var("OUT_DIR").unwrap());

    bindgen::Builder::default()
        .header("wrapper.h")
        .clang_arg("-xc++")//Needed since bidgen this the header is C
        .parse_callbacks(Box::new(bindgen::CargoCallbacks))
        .generate()
        .expect("bindgen should sucessfully generate bindings")
        .write_to_file(output_path.join("bindings.rs"))
        .expect("bindgen should sucessfully write bindings to disk");
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
