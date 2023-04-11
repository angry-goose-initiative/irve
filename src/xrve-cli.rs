/* NAME//TODO
 * By: John Jekel
 *
 * TODO description
 *
*/

/* Imports */

use xrve::Instance;

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
    let mut system = Instance::new();

    let log_receiver = system.get_log_receiver();

    //TESTING
    system.single_step();
    system.run_in_thread();
    for _ in 0..30 {
        let (log_level, message) = log_receiver.recv().unwrap();
        println!("{:?}: {}", log_level, message);
    }

    todo!();
}
