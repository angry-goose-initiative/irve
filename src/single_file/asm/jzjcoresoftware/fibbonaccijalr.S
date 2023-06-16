#include "rvsw_asm.h"

    # x29 is x, x30 is y, x31 is z
    
    # register setup
    addi x29, x0, 0
    addi x30, x0, 1
    addi x31, x0, 0
    
    #loop
    #get next term
    add x31, x29, x30
    
    # shift terms back
    addi x29, x30, 0
    addi x30, x31, 0
    
    #jump to start of loop
    jalr x0, x0, 12
    
    #invalid code will be displayed if we reach here because jal failed
    addi x31, x0, 4
    RVSW_EXIT_ASM
