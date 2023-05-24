/* build.rs
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Build script for irver
 *
 * Based on cbindgen and bindgen examples
*/

/* Imports */

extern crate bindgen;

/* Function Implementations */

fn main() {
    println!("cargo:rustc-link-lib=static=irve");
    println!("cargo:rustc-link-lib=static=irve_disassemble");
    println!("cargo:rustc-link-lib=stdc++");//TODO will this break if the system only has Clang's libc++ and not GNU's libstdc++?
    println!("cargo:rustc-link-search=./");

    println!("cargo:rerun-if-changed=wrapper.h");
    println!("cargo:rerun-if-changed=../../include/irve_disassemble.h");

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

/* Tests */

#[cfg(test)]
mod tests {
    use super::*;

    //#[test]
    //TODO
}
