/* logging.rs
 * By: John Jekel
 *
 * Logging facilities for XRVE
 *
*/

/* Imports */

use std::sync::mpsc;

/* Modules */

pub mod prelude {
    pub use xrve_proc_macro::log;
    pub(crate) use crate::logging::log_with;
    pub use crate::logging::LogLevel;
    pub use crate::logging::Logger;
}

/* Macros */

macro_rules! log_with {
    //Level could be a LogLevel or a u8 for the verbosity of Info
    ($logger:expr, $level:expr, $($format_args:expr),*) => {
        #[cfg(feature = "logging")]
        if let Some(log_sender) = $logger.as_mut() {
            log_sender.send(($level.into(), format!($($format_args),*))).unwrap();
        }
    };
    //Defaults to Debug
    ($logger:expr, $($format_args:expr),*) => {
        #[cfg(feature = "logging")]
        if let Some(log_sender) = $logger.as_mut() {
            log_sender.send((crate::logging::LogLevel::Debug, format!($($format_args),*))).unwrap();
        }
    };
}
pub(crate) use log_with;

/* Static Variables */

//TODO

/* Types */

#[derive(Debug)]
pub enum LogLevel {
    Error,
    Warning,
    Info(u8),//Verbosity
    Debug
}

pub type Logger = Option<mpsc::Sender<(LogLevel, String)>>;
pub type LogReciever = mpsc::Receiver<(LogLevel, String)>;

/* Associated Functions and Methods */

impl From<u8> for LogLevel {
    fn from(value: u8) -> Self {
        return LogLevel::Info(value);
    }
}

/* Functions */

pub fn init_logging() -> (Logger, LogReciever) {
    let (sender, reciever) = mpsc::channel();
    let mut logger = Some(sender);
    log_with!(logger, 0, "XRVE Log started");
    (logger, reciever)
}
