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
    #jal x0, -12
    #j -12
    #assembler not playing nice so we can't use jal directly. Assembled by hand instead
    .word 0xff5ff06f
    
    #invalid code will be displayed if we reach here because jal failed
    addi x31, x0, 4
