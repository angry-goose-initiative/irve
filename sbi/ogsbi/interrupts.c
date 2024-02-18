/**
 * @file    interrupts.c
 * @brief   Entry point back into M mode upon recieving an interrrupt
 * 
 * @copyright
 *  Copyright (C) 2024 John Jekel\n
 *  See the LICENSE file at the root of the project for licensing info.
 * 
 * NOTE: We delegate all S-Mode interrupts to S-Mode, so only M-Mode interrupts need be handled here
 *
 * NOTE: Unlike exceptions where we can print things out for debugging, we can't do that here since
 * print functions may not be re-entrant, and we're already using them for exceptions. Yes this 
 * shouldn't matter since a trap to m-mode would disable interrupts, but it's better to be safe than sorry.
*/

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include "rvsw.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

__attribute__ ((interrupt ("machine"))) void ___rvsw_machine_software_interrupt_handler___(void) {
    //Not implemented since only OGSBI can set the software interrupt pending bit, but we don't use
    //it for anything (yet)
    assert(false && "TODO implement ___rvsw_machine_software_interrupt_handler___");
    exit(1);
}

__attribute__ ((interrupt ("machine"))) void ___rvsw_machine_timer_interrupt_handler___(void) {
    //We simply set the supervisor timer interrupt pending bit and return, thereby passing
    //the timer interrupt to the supervisor transparently since hardware only causes m-mode interrupts
    uint32_t new_mip = RVSW_CSRR(mip);
    new_mip |= (1 << 5);//Set the supervisor timer interrupt pending bit
    new_mip &= ~(1 << 7);//Clear the machine timer interrupt pending bit
    RVSW_CSRW(mip, new_mip);
    
    //Returning will re-enable interrupts and jump to the supervisor's timer interrupt handler
}

__attribute__ ((interrupt ("machine"))) void ___rvsw_machine_external_interrupt_handler___(void) {
    //We simply set the supervisor external interrupt pending bit and return, thereby passing
    //the external interrupt to the supervisor transparently since hardware only causes m-mode interrupts
    uint32_t new_mip = RVSW_CSRR(mip);
    new_mip |= (1 << 9);//Set the supervisor external interrupt pending bit
    new_mip &= ~(1 << 11);//Clear the machine external interrupt pending bit
    RVSW_CSRW(mip, new_mip);
    
    //Returning will re-enable interrupts and jump to the supervisor's external interrupt handler
}
