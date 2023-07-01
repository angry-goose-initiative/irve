/* irve_exception_bringup.c
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * A testfile used for exception bringup.
 *
*/

/* Constants And Defines */

#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

/* Macros */

#define csrrw(destination, csr, source) do { \
    __asm__ volatile ( \
        "csrrw %[rd], " #csr ", %[rs1]" \
        : [rd] "=r" (destination) \
        : [rs1] "r" (source) \
        : /* No clobbered registers */ \
    ); \
} while (0)

#define csrrs(destination, csr, source) do { \
    __asm__ volatile ( \
        "csrrs %[rd], " #csr ", %[rs1]" \
        : [rd] "=r" (destination) \
        : [rs1] "r" (source) \
        : /* No clobbered registers */ \
    ); \
} while (0)

#define csrrc(destination, csr, source) do { \
    __asm__ volatile ( \
        "csrrc %[rd], " #csr ", %[rs1]" \
        : [rd] "=r" (destination) \
        : [rs1] "r" (source) \
        : /* No clobbered registers */ \
    ); \
} while (0)

#define csrrwi(destination, csr, imm) do { \
    __asm__ volatile ( \
        "csrrwi %[rd], " #csr ", " #imm \
        : [rd] "=r" (destination) \
        : /* No source registers */ \
        : /* No clobbered registers */ \
    ); \
} while (0)

#define csrrsi(destination, csr, imm) do { \
    __asm__ volatile ( \
        "csrrsi %[rd], " #csr ", " #imm \
        : [rd] "=r" (destination) \
        : /* No source registers */ \
        : /* No clobbered registers */ \
    ); \
} while (0)

#define csrrci(destination, csr, imm) do { \
    __asm__ volatile ( \
        "csrrci %[rd], " #csr ", " #imm \
        : [rd] "=r" (destination) \
        : /* No source registers */ \
        : /* No clobbered registers */ \
    ); \
} while (0)

#define csrrs_read(destination, csr) do { \
    __asm__ volatile ( \
        "csrrs %[rd], " #csr ", x0" \
        : [rd] "=r" (destination) \
        : /* No source registers */ \
        : /* No clobbered registers */ \
    ); \
} while (0)

#define csrrc_read(destination, csr) do { \
    __asm__ volatile ( \
        "csrrc %[rd], " #csr ", x0" \
        : [rd] "=r" (destination) \
        : /* No source registers */ \
        : /* No clobbered registers */ \
    ); \
} while (0)

#define csrrsi_read(destination, csr) do { \
    __asm__ volatile ( \
        "csrrsi %[rd], " #csr ", 0" \
        : [rd] "=r" (destination) \
        : /* No source registers */ \
        : /* No clobbered registers */ \
    ); \
} while (0)

#define csrrci_read(destination, csr) do { \
    __asm__ volatile ( \
        "csrrci %[rd], " #csr ", 0" \
        : [rd] "=r" (destination) \
        : /* No source registers */ \
        : /* No clobbered registers */ \
    ); \
} while (0)

/* Static Variables */

static volatile struct {
    uint32_t mstatus;
    uint32_t mcause;
    uint32_t mepc;
    uint32_t mtval;
} last_exception_info;

/* Function Implementations */

int main() {
    puts("Exception Bringup Tests");

    puts("Delegate all to machine mode");
    __asm__ volatile ("csrw medeleg, zero");

    puts("Start with mstatus = 0x00000000");
    uint32_t zero = 0x00000000;
    csrrw(zero, mstatus, zero);

    puts("\n********** Some initial experiments **********");/////////////////////////////////////////////////////////////////////

    *((volatile uint32_t*)0xDEADBEEF) = 0x12345678;//Access to an unmapped address should cause an exception
    //TODO mstatus assertion that we came from machine mode
    assert((last_exception_info.mcause == 0x00000006) && "Exception cause was not a store/AMO address misaligned exception");
    //assert(last_exception_info.mepc == ???);//TODO we'll have to test this with pure assembly since we don't now the address of the instruction that caused the exception

    puts("\n********** Now for some more comprehensive assertions **********");/////////////////////////////////////////////////////////////////////

    puts("TODO");

    puts("All assertions passed! You made it! :)");
    return 0;
}

static void actually_handle(void) {
    switch (last_exception_info.mcause) {
        case 0x00000006:
            puts("Store/AMO address misaligned");
            uint32_t instruction = *((volatile uint32_t*)last_exception_info.mepc);
            printf("    Problematic instruction: 0x%08lX\n", instruction);
            uint32_t _;
            csrrw(_, mepc, last_exception_info.mepc + 4);//Just skip the instruction
            puts("    Skipping the instruction");
            break;
        default:
            assert(false && "TODO");
    }
}

__attribute__ ((interrupt ("machine"))) void __riscv_synchronous_exception_and_user_mode_swi_handler(void) {
    puts("Recieved exception!");
    csrrsi_read(last_exception_info.mstatus, mstatus);
    csrrsi_read(last_exception_info.mcause, mcause);
    csrrsi_read(last_exception_info.mepc, mepc);
    csrrsi_read(last_exception_info.mtval, mtval);
    printf("    mstatus:    0x%08lX\n", last_exception_info.mstatus);
    printf("    mcause:     0x%08lX\n", last_exception_info.mcause);
    printf("    mepc:       0x%08lX\n", last_exception_info.mepc);
    printf("    mtval:      0x%08lX\n", last_exception_info.mtval);
    actually_handle();
}
