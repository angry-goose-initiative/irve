/* logging.rs
 * By: John Jekel
 *
 * Logging facilities for XRVE
 *
*/

/* Imports */

pub use xrve_proc_macro::log;
use std::sync::mpsc;

/* Constants */

//TODO

/* Macros */

//TODO add compile time option to disable logging for better performance (just have the log! macro do nothing)
//TODO (also pub(crate) use the_macro statements here too)
macro_rules! log_with {
    //Level could be a LogLevel or a u8 for the verbosity of Info
    ($logger:expr, $level:expr, $($format_args:expr),*) => {
        if let Some(log_sender) = $logger.as_mut() {
            log_sender.send(($level.into(), format!($($format_args),*))).unwrap();
        }
    };
    //Defaults to Debug
    ($logger:expr, $($format_args:expr),*) => {
        if let Some(log_sender) = $logger.as_mut() {
            log_sender.send((crate::logging::LogLevel::Debug, format!($($format_args),*))).unwrap();
        }
    };
}
pub(crate) use log_with;

macro_rules! use_logging {
    () => {
        use crate::logging::log;
        use crate::logging::log_with;
        use crate::logging::Logger;
    };
}
pub(crate) use use_logging;

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
