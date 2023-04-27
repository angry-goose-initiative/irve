/* NAME//TODO
 * By: John Jekel
 *
 * TODO description
 *
*/

//TODO better organization of everything

/* Imports */

use proc_macro::TokenStream;
use proc_macro::TokenTree;
use proc_macro::Literal;
use std::str::FromStr;

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

//TODO put these into seperate modules

#[proc_macro]
pub fn unstringify(input_tokens: TokenStream) -> TokenStream {
    //Get the string literal token from the input tokens
    debug_assert!(input_tokens.clone().into_iter().count() == 1, "compile_time_parse!() must only be passed a single token (a string)");
    let token = input_tokens.into_iter().next().expect("The token should exist due to the assertion above");
    debug_assert!(matches!(token, TokenTree::Literal(_)), "compile_time_parse!() must only be passed a single token (a string)");

    //Extract the string from the Literal and remove the surrounding quotes
    let token_as_string = token.to_string();
    let mut token_as_string_iter = token_as_string.chars();
    let (first_char, last_char) = (token_as_string_iter.next(), token_as_string_iter.next_back());
    debug_assert!(first_char == Some('"') && last_char == Some('"'), "compile_time_parse!() must only be passed a single token (a string)");
    let string_contents = token_as_string_iter.as_str();

    //Deal with escape sequences
    //TODO handle more of these
    let string = string_contents.replace("\\\"", "\"");

    //Construct and return a new TokenStream
    let new_token_stream_wrapped = TokenStream::from_str(&string);
    debug_assert!(new_token_stream_wrapped.is_ok(), "Contents of string had invalid syntax");
    new_token_stream_wrapped.expect("The TokenStream should exist due to the assertion above")
}

#[proc_macro]
pub fn log(input_tokens: TokenStream) -> TokenStream {
    //"TESTING".parse().unwrap()
    //"compile_error!(\"TESTING\")".parse().unwrap()
    //TODO
    //todo!();
    //TokenStream::new()
    //if let Some(log_sender) = $logger.as_mut() {
        //log_sender.send(($level.into(), format!($($format_args),*))).unwrap();
    //}
    let mut log_command = "crate::logging::log_with!(l, ".to_string();
    log_command.push_str(input_tokens.to_string().as_str());
    log_command.push_str(")");
    log_command.parse().unwrap()
}
