/* irve_newlib.c
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Implementation of Newlib syscalls and setup/teardown for IRVE Machine Mode programs
 *
 * Useful resources (partially based on):
 * https://www.embecosm.com/appnotes/ean9/ean9-howto-newlib-1.0.html#sec_syscalls
 * https://interrupt.memfault.com/blog/boostrapping-libc-with-newlib#implementing-our-own-c-standard-library
*/

/* Includes */

#include <errno.h>
#include <sys/stat.h>

#include <stdbool.h>

#include "irve.h"

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

void __pre_main(void) {
    //Initialize the C library
    __libc_init_array();

    //TODO must we do anything else here?
}

void __post_main(void) {
    //TODO must we do anything here?
    //TODO we should be calling "destructors" (the c atexit() function) before we halt?
}

//TODO implement others eventually

int _close(int) {
    errno = EBADF;
    return -1;
}

void _exit(int) {
    irve_exit();
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

int _read(int, char*, int len) {
    return 0;//Don't support stdin for now
}

int _write(int file, char* str, int len) {
    //NOTE: file is ignored since we only support stdout
    for (int i = 0; i < len; i++) {
        IRVE_DEBUG_ADDR = str[i];
    }
}
