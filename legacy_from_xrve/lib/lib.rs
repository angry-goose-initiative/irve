/* lib.rs
 * By: John Jekel
 *
 * The eXtensible RISC-V Emulator's library
 *
*/

/*!
 *
 * The eXtensible RISC-V Emulator's library
 *
 * //TODO
 *
*/

/* Imports */

pub mod about;
mod instruction_handler;
mod csr_handler;
mod fetch;
mod decode;
mod instance;
mod io;
mod logging;
mod state;
pub mod pmmap;
pub mod standard_extensions;

pub use instance::Instance;
pub use logging::LogReciever;
pub use logging::LogLevel;

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

/* Functions */

//TODO

/* Tests */

//TODO
