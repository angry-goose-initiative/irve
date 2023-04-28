/* NAME//TODO
 * By: John Jekel
 *
 * TODO description
 *
*/

/* Constants And Defines */

//TODO

/* Includes */

#include "emulator.h"

#define INST_COUNT this->get_inst_count()
#include "logging.h"

/* Types */

//TODO

/* Variables */

//TODO

/* Static Function Declarations */

//TODO

/* Function Implementations */

emulator_t::emulator_t(): cpu_state(), memory() {
    irvelog(0, "Created new emulator instance");
}

void emulator_t::tick() {
    this->cpu_state.increment_inst_count();
    irvelog(0, "Tick begins");
    //TODO
}

uint64_t emulator_t::get_inst_count() const {
    return cpu_state.get_inst_count();
}

/* Static Function Implementations */

//TODO
