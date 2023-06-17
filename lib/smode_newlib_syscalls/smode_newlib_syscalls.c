/* smode_newlib_syscalls.c
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Implementation of Newlib syscalls
 *
 * Useful resources (partially based on):
 * https://sourceware.org/newlib/libc.html#Syscalls
 * https://www.embecosm.com/appnotes/ean9/ean9-howto-newlib-1.0.html#sec_syscalls
 * https://interrupt.memfault.com/blog/boostrapping-libc-with-newlib#implementing-our-own-c-standard-library
 * IRVE M-Mode syscalls
*/

/* Includes */

#include <errno.h>
#include <sys/stat.h>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

/* Variables */

#undef errno
extern int errno;

/* Function Declarations */

extern void __libc_init_array(void);

void __pre_main(void);
void __post_main(void);

int _close(int file);
void _exit(int return_code);
int _fstat(int file, struct stat* the_stats);
int _getpid(void);
int _isatty(int file);
int _kill(int pid, int sig);
int _lseek(int file, int ptr, int dir);
int _read(int file, char* str, int len);
int _write(int file, char* str, int len);

/* Function Implementations */

//TODO implement others eventually

int _close(int) {
    errno = EBADF;
    return -1;
}

//[[noreturn]]//FIXME this requires GCC 13
__attribute__((noreturn))
void _exit(int) {
    //FIXME don't use legacy shutdown request
    __asm__ volatile (
        "li a7, 0x08"
        "ecall"
    );

    //irve_mmode_exit();
    while (true);//TODO implement with SBI call
}

int _fstat(int, struct stat* the_stats) {
    //Tell Newlib all files are character devices
    //This forces it to read/write one char at a time
    the_stats->st_mode = S_IFCHR;
    return 0;
}

int _getpid(void) {
    return 1;//We don't support multiple processes
}

int _isatty(int) {
    return 1;//Since we only support stdout output via IRVE_DEBUG_ADDR
}

int _kill(int pid, int) {
    //We don't support processes other than pid 1
    if (pid == 1) {
        _exit(0);
    } else {
        errno = EINVAL;
        return -1;
    }
}

int _lseek(int, int, int) {
    return 0;//Again, just stdout, which doesn't support seeking
}

int _read(int, char*, int) {
    return 0;//Don't support stdin for now
}

int _write(int, char* str, int len) {
    //FIXME don't use legacy console putchar
    /*
    __asm__ volatile (
        "li a7, 0x08"
        "ecall"
    );
    */

    //TODO implement with SBI call
    //NOTE: file is ignored since we only support stdout
    /*for (int i = 0; i < len; ++i) {
        IRVE_MMODE_DEBUG_ADDR = str[i];
    }

    return len;
    */
    for (int i = 0; i < len; ++i) {
        *((volatile char*)(0xFFFFFFFF)) = str[i];//TESTING
    }

    return len;
}
