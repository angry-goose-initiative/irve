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
    puts("Hello World printed with puts from Newlib!");
    fflush(stdout);
    return 0;
}

//The following functions implementations are partly original
//They may be partially based on:
//https://sourceware.org/newlib/libc.html#Syscalls
//https://www.embecosm.com/appnotes/ean9/ean9-howto-newlib-1.0.html#sec_syscalls
//https://interrupt.memfault.com/blog/boostrapping-libc-with-newlib#implementing-our-own-c-standard-library
#undef errno
extern int errno;

int _close(int) {
    errno = EBADF;
    return -1;
}

int _fstat(int, struct stat* st) {
    //Tell Newlib all files are character devices
    //This forces it to read/write one char at a time
    st->st_mode = S_IFCHR;
    return  0;
}

int _isatty(int) {
    return 1;//Since we only support stdout output via IRVE_DEBUG_ADDR
}

int _lseek(int, int, int) {
    return 0;//Again, just stdout, which doesn't support seeking
}

int _read(int, char*, int) {
    return 0;//Don't support stdin for now
}

int _write(int, char* ptr, int len) {
    //NOTE: file is ignored since we only support stdout
    for (int i = 0; i < len; i++) {
        IRVE_DEBUG_ADDR = ptr[i];
    }

    return len;
}
