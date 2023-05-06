/* hello_newlib.c
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Hello World using Newlib
 *
 * Compiled with: /opt/riscv/bin/riscv32-unknown-elf-gcc -march=rv32ima -mabi=ilp32 -nostartfiles -static -static-libgcc --specs=nosys.specs -lc -lgcc -mstrict-align -T ../irve.ld ./crt0.s ./with_c_stdlib/hello_newlib.c && /opt/riscv/bin/riscv32-unknown-elf-objcopy ./a.out -O verilog --verilog-data-width=4 ../../precompiled/hello_newlib.txt
 *
 * DOES NOT USE irve_newlib.c
 *
 * Based on code from rv32esim
*/

/* Includes */

#include <errno.h>
#include <sys/stat.h>

#include "irve.h"

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>

/* Function Declarations */

int _close(int file);
int _fstat(int file, struct stat *st);
int _isatty(int file);
int _lseek(int file, int ptr, int dir);
int _read(int file, char *ptr, int len);
int _write(int file, char *ptr, int len);

/* Function Implementations */

int main() {
    puts("Hello World printed with puts from Newlib!\n");
}

//Called if an assertion fails
/*void __assert_func(const char* file, int line, const char* function, const char* expr) {
    print_string("Assertion failed: ");
    print_string(file);
    print_string(" | ");
    print_string(function);
    print_string(" | ");
    print_string(expr);
    print_string("\n");
    __asm__ volatile ("ecall");
    while (true);
}*/

//The following functions implementations are partly original
//They may be partially based on:
//https://www.embecosm.com/appnotes/ean9/ean9-howto-newlib-1.0.html#sec_syscalls
//https://interrupt.memfault.com/blog/boostrapping-libc-with-newlib#implementing-our-own-c-standard-library
#undef errno
extern int errno;

int _close(int file) {
    errno = EBADF;
    return -1;
}

int _fstat(int file, struct stat *st) {
    //Tell Newlib all files are character devices
    //This forces it to read/write one char at a time
    st->st_mode = S_IFCHR;
    return  0;
}

int _isatty(int file) {
    return 1;//Since we only support stdout output via IRVE_DEBUG_ADDR
}

int _lseek(int file, int ptr, int dir) {
    return 0;//Again, just stdout, which doesn't support seeking
}

int _read(int file, char *ptr, int len) {
    return 0;//Don't support stdin for now
}

int _write(int file, char *ptr, int len) {
    //NOTE: file is ignored since we only support stdout
    for (int i = 0; i < len; i++) {
        IRVE_DEBUG_ADDR = ptr[i];
    }
}
