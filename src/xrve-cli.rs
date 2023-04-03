/* NAME//TODO
 * By: John Jekel
 *
 * TODO description
 *
*/

/* Imports */

use xrve::System;

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

fn main() {
    let mut system = System::new();

    let log_receiver = system.get_log_receiver();

    //TESTING
    loop {
        println!("{}", log_receiver.recv().unwrap());
    }

    todo!();
}
