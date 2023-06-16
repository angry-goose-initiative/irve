#include "rvsw_asm.h"

    #loop
    #load byte in data section into register 31
    lhu x31, 12(x30)
    
    #go to next address
    addi x30, x30, 2
    
    #go back to start
    jalr x0, x0, 0
    
    #Data
    .byte 0x01
    .byte 0x23
    .byte 0x45
    .byte 0x67
    .byte 0x89
    .byte 0xab
    .byte 0xcd
    .byte 0xef
    .byte 0xfe
    .byte 0xdc
    .byte 0xba
    .byte 0x98
    .byte 0x76
    .byte 0x54
    .byte 0x32
    .byte 0x10
