/* csr_bringup.c
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * A testfile used for CSR bringup.
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
        "csrrci %[rd], " #csr ", " #imm \
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

/* Function Implementations */

int main() {
    puts("CSR Bringup Tests");

    puts("\nSome initial experiments");/////////////////////////////////////////////////////////////////////

    //TODO test even unimplemented instructions so that we can fix our mistakes until everything is implemented

    puts("Testing csrrs");
    uint32_t result;
    uint32_t source = 0;//NOTE: This is not the same as x0, since it still "writes" to the CSR
    csrrs(result, 0xB02, source);
    printf("minstret: %lu\n", result);
    csrrs_read(result, minstret);
    printf("minstret without write: %lu\n", result);
    source = 0xFFFFFFFF;
    csrrs(result, mscratch, source);
    csrrs_read(result, mscratch);
    printf("mscratch was set to: 0x%08lX\n", result);
    assert((result == 0xFFFFFFFF) && "mscratch was not set to 0xFFFFFFFF");

    puts("Testing csrrc");
    csrrc(result, 0xB02, source);
    printf("minstret: %lu\n", result);
    csrrs_read(result, minstret);
    printf("minstret without write: %lu\n", result);
    source = 0xFFFFFFFF;
    csrrc(result, mscratch, source);
    csrrc_read(result, mscratch);
    printf("mscratch was set to: 0x%08lX\n", result);
    assert((result == 0) && "mscratch was not set to 0");

    puts("TODO");

    puts("Testing csrrw");

    puts("TODO");

    puts("\nNow for some assertions");/////////////////////////////////////////////////////////////////////

    return 0;
}

__attribute__ ((interrupt ("machine"))) void __riscv_synchronous_exception_and_user_mode_swi_handler(void) {
    assert(false && "We only access CSRs in a valid manner, so we should never get here");
}
