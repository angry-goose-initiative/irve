entry:
    addi x31, x31, 10
    #This isen't a problem with the cpu, but I think with the assembler. It sucessfully translates call to auipc and jalr, but it does not put the proper offset in jalr (puts 0x0, should put 0xc)
    call functiontest
    ecall
    
functiontest:
    addi x31, x31, 17
    ret
