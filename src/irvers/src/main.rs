/* main.rs
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Entry point for irvers
 *
*/

/* Imports */

mod irve;//We do NOT use ffi.rs directly (it is very unsafe)

use irve::logging::irvelog;
use irve::logging::irvelog_always;

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
    //TODO get boot time here

    irvelog_always!(0, "\x1b[1mStarting \x1b[94mIRVE\x1b[0m (Rust Frontend)");
    irvelog!(0, "\x1b[1m\x1b[94m ___ ______     _______ \x1b[0m");
    irvelog!(0, "\x1b[1m\x1b[94m|_ _|  _ \\ \\   / / ____|\x1b[0m");
    irvelog!(0, "\x1b[1m\x1b[94m | || |_) \\ \\ / /|  _|  \x1b[0m"); 
    irvelog!(0, "\x1b[1m\x1b[94m | ||  _ < \\ V / | |___ \x1b[0m");
    irvelog!(0, "\x1b[1m\x1b[94m|___|_| \\_\\ \\_/  |_____|\x1b[0m");
    irvelog!(0, "");
    irvelog!(0, "\x1b[1mThe Inextensible RISC-V Emulator\x1b[0m");
    irvelog_always!(0, "\x1b[1mCopyright (C) 2023 \x1b[95mJohn Jekel\x1b[37m and \x1b[92mNick Chan\x1b[0m");
    irvelog_always!(0, "\x1b[90mSee the LICENSE file at the root of the project for licensing info.\x1b[0m");
    irvelog!(0, "");
    irvelog_always!(0, "\x1b[90mlibirve {}\x1b[0m", irve::about::get_version_string());
    irvelog_always!(0, "\x1b[90mlibirve built at {} on {}\x1b[0m", irve::about::get_build_time_string(), irve::about::get_build_date_string());
    //irvelog_always!(0, "\x1b[90mirvers executable built at {} on {}\x1b[0m", __TIME__, __DATE__);//TODO
    irvelog!(0, "\x1b[90mBuild System: {}\x1b[0m", irve::about::get_build_system_string());
    irvelog!(0, "\x1b[90mBuilt from {} for {}\x1b[0m", irve::about::get_build_host_string(), irve::about::get_compile_target_string());
    irvelog!(0, "\x1b[90mCompiler: {}\x1b[0m", irve::about::get_compiler_string());
    irvelog!(0, "------------------------------------------------------------------------");
    irvelog!(0, "");
    irvelog!(0, "");

    todo!()
}

/* Tests */

//TODO
