/* NAME//TODO
 * By: John Jekel
 *
 * TODO description
 *
*/

/* Imports */

use xrve::Instance;
use xrve::pmmap::ram::Ram;
use xrve::standard_extensions::i;

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

    let ram = Ram::new(0, 1000);

    system.register_memory_handler(ram);
    system.register_instruction_handler(i::OpInstructionHandler::new());
    system.register_instruction_handler(i::OpImmInstructionHandler::new());

    //TESTING load nop into memory
    system.write_word_to_memory(0, 0x00000013).expect("Writing word to address in Ram which should exist");

    //TESTING
    system.single_step();
    system.run_in_thread();
    std::thread::sleep(std::time::Duration::from_millis(1));
    system.stop_thread();
    while let Ok((log_level, message)) = log_receiver.try_recv() {
        println!("{:?}: {}", log_level, message);
    }

    todo!();
}

/* Tests */

//TODO
