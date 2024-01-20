/**
 * @file    irve_csr_bringup.c
 * @brief   A testfile used for CSR bringup (CSR instructions, not specific CSR registers quite yet)
 * 
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  See the LICENSE file at the root of the project for licensing info.
*/

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

/* ------------------------------------------------------------------------------------------------
 * Macros
 * --------------------------------------------------------------------------------------------- */

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

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

int main() {
    puts("CSR (instruction) Bringup Tests");

    puts("\n********** Some initial experiments **********");/////////////////////////////////////////////////////////////////////

    puts("Testing csrrs");
    uint32_t result;
    uint32_t source = 123456;//NOTE: This is not the same as x0, since it still "writes" to the CSR
    csrrw(result, 0xB02, source);
    printf("minstret before overwrite (now overwritten): %lu\n", result);
    csrrw(result, 0xB02, source);
    printf("minstret after overwrite (now restored): %lu\n", result);
    assert((result >= 123456) && "minstret was not written to correctly by the first csrrw");
    puts("Testing csrrw");

    puts("Testing csrrs");
    source = 0;//NOTE: This is not the same as x0, since it still "writes" to the CSR
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

    puts("Testing csrrwi");
    csrrwi(result, mscratch, 0x12);
    printf("mscratch before overwrite (now overwritten): 0x%08lX\n", result);
    assert((result == 0) && "csrrwi did not return 0");
    csrrwi(result, mscratch, 0x11);
    printf("minstret after overwrite (now overwritten again): 0x%08lX\n", result);
    assert((result == 0x12) && "mscratch was not written to correctly by the first csrrw");

    puts("Testing csrrsi");
    csrrsi(result, mscratch, 0x5);
    printf("mscratch before csrrsi: 0x%08lX\n", result);
    assert((result == 0x11) && "csrrsi did not return 0x11");
    csrrsi_read(result, mscratch);
    printf("mscratch after csrrsi: 0x%08lX\n", result);
    assert((result == 0x15) && "mscratch was not written correctly");

    puts("Testing csrrci");
    csrrci(result, mscratch, 0xD);
    printf("mscratch before csrrci: 0x%08lX\n", result);
    assert((result == 0x15) && "csrrci did not return 0x15");
    csrrci_read(result, mscratch);
    printf("mscratch after csrrci: 0x%08lX\n", result);
    assert((result == 0x10) && "mscratch was not written correctly");

    puts("\n********** Now for some more comprehensive assertions **********");/////////////////////////////////////////////////////////////////////

    source = 0x12345678;
    csrrw(result, mscratch, source);
    source = 0xABCD1234;
    csrrw(result, mscratch, source);
    assert(result == 0x12345678);
    source = 0x00FFFF00;
    csrrs(result, mscratch, source);
    assert(result == 0xABCD1234);
    csrrs_read(result, mscratch);
    assert(result == 0xABFFFF34);
    source = 0x000FF000;
    csrrc(result, mscratch, source);
    assert(result == 0xABFFFF34);
    csrrc_read(result, mscratch);
    assert(result == 0xABF00F34);
    csrrwi(result, mscratch, 0xF);
    assert(result == 0xABF00F34);
    csrrwi(result, mscratch, 0x0);
    assert(result == 0xF);
    csrrsi(result, mscratch, 0x3);
    assert(result == 0x0);
    csrrsi_read(result, mscratch);
    assert(result == 0x3);
    csrrci(result, mscratch, 0x1);
    assert(result == 0x3);
    csrrci_read(result, mscratch);
    assert(result == 0x2);

    puts("All assertions passed! You made it! :)");
    return 0;
}

__attribute__ ((interrupt ("machine"))) void ___rvsw_exception_handler___(void) {
    assert(false && "We only access CSRs in a valid manner, so we should never get here");
    exit(1);
}
