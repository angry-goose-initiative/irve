entry:
    addi x31, x31, 10
    call functiontest
    .insn r CUSTOM_0, 0, 0, zero, zero, zero#Exit
    
functiontest:
    addi x31, x31, 17
    ret
