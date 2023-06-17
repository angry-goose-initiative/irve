/* main.rs
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Entry point for irvers
 *
*/

/* Imports */

use irve::logging::irvelog;
use irve::logging::irvelog_always;
use irve::emulator::Emulator;

use std::time::Instant;

/* Constants */

const TESTFILES_DIR: &str = "rvsw/compiled/";

/* Functions */

fn main() {
    let irve_boot_time = Instant::now();

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
   
    irvelog_always!(0, "Initializing emulator...");
    let mut emulator = Emulator::new();

    let args: Vec<String> = std::env::args().collect();

    if args.len() == 1 {
        irvelog_always!(0, "No memory image file specified. Starting with empty memory.");
    } else {
        assert!(args.len() == 2, "Too many arguments for now");//TODO remove this if we need in the future

        //Locate and the image file (guessing it if it is not a whole path for convenience)
        let mem_file: String;
        //A testfile name rather than a path, so prepend the testfiles directory
        if args[1].contains("/") {
            mem_file = args[1].clone();
        } else {
            mem_file = TESTFILES_DIR.to_owned() + &args[1];
        }

        irvelog_always!(0, "Loading memory image from file \"{}\"", mem_file);
        emulator.load_verilog_32(&mem_file);
    }

    let init_time_us = irve_boot_time.elapsed().as_micros();
    irvelog_always!(0, "Initialized the emulator in {}us", init_time_us);

    let execution_start_time = Instant::now();
    emulator.run_until(0);//Run the emulator until we get an exit request

    let execution_time_us = execution_start_time.elapsed().as_micros();
    irvelog_always!(0, "Emulation finished in {}us", execution_time_us);
    irvelog_always!(0, "{} instructions were executed", emulator.get_inst_count());
    let average_ips = (emulator.get_inst_count() as f64) / (execution_time_us as f64) * 1000000.0;
    irvelog_always!(0, "Average of {} instructions per second ({}MHz)", average_ips, (average_ips / 1000000.0));

    irvelog_always!(0, "\x1b[1mIRVE is shutting down. Bye bye!\x1b[0m");
}

/* Tests */

#[cfg(test)]
mod tests {
    use super::*;

    //#[test]
    //TODO
}