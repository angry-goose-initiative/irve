/**
 * @file    irve_interrupt_bringup.cpp
 * @brief   A testfile used for interrupt bringup.
 * 
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  See the LICENSE file at the root of the project for licensing info.
*/

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include <cassert>
#include <cstdio>
#include <cstdlib>

/* ------------------------------------------------------------------------------------------------
 * Static Variables
 * --------------------------------------------------------------------------------------------- */

static volatile bool supervisor_mode_software_interrupt_handler_called  = false;
static volatile bool supervisor_mode_timer_interrupt_handler_called     = false;
static volatile bool supervisor_mode_external_interrupt_handler_called  = false;

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

int main(int, const char**) {
    puts("IRVE Interrupt Bringup Tests");

    puts("Delegate all interrupts to machine mode");
    __asm__ volatile ("csrw mideleg, zero");

    puts("Set mstatus.MIE");
    __asm__ volatile ("csrsi mstatus, (1 << 3)");

    puts("Enable all S-mode interrupts and set them as pending");
    //Note: We can set M-mode interrupts as pending, so for this test we'll just focus on S-mode interrupts
    __asm__ volatile (
        "li t0, 0b00000000000000000000001000100010\n"
        "csrw mie, t0\n"
        "csrw mip, t0\n"
        : /* No destination registers */
        : /* No source registers */
        : "t0"
    );

    while (!supervisor_mode_software_interrupt_handler_called);
    puts("Supervisor mode software interrupt handler called");

    while (!supervisor_mode_timer_interrupt_handler_called);
    puts("Supervisor mode timer interrupt handler called");

    while (!supervisor_mode_external_interrupt_handler_called);
    puts("Supervisor mode external interrupt handler called");

    return 0;
}

extern "C" __attribute__ ((interrupt ("machine"))) void ___rvsw_exception_handler___(void) {
    assert(false && "We don't expect any exceptions to occur in this test program");
    exit(1);
}

//Note: We CANNOT call standard library functions from interrupt handlers

//Also note we still use `interrupt ("machine")` even though they're supervisor mode interrupts
//This is because we're HANDLING them in machine mode, not because they're machine mode interrupts

extern "C" __attribute__ ((interrupt ("machine"))) void ___rvsw_supervisor_software_interrupt_handler___(void) {
    supervisor_mode_software_interrupt_handler_called = true;

    //Clear the pending bit
    __asm__ volatile ("csrc mip, (1 << 1)");
}

extern "C" __attribute__ ((interrupt ("machine"))) void ___rvsw_supervisor_timer_interrupt_handler___(void) {
    supervisor_mode_timer_interrupt_handler_called = true;

    //Clear the pending bit
    __asm__ volatile (
        "li t0, (1 << 5)\n"
        "csrc mip, t0\n"
        : /* No destination registers */
        : /* No source registers */
        : "t0"
    );
}

extern "C" __attribute__ ((interrupt ("machine"))) void ___rvsw_supervisor_external_interrupt_handler___(void) {
    supervisor_mode_external_interrupt_handler_called = true;

    //Clear the pending bit
    __asm__ volatile (
        "li t0, (1 << 9)\n"
        "csrc mip, t0\n"
        : /* No destination registers */
        : /* No source registers */
        : "t0"
    );
}
