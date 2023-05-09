entry:
    addi x31, x0, 10
    jal x1, functiontest
    .insn r CUSTOM_0, 0, 0, x0, x0, x0#Exit
    
functiontest:
    addi x31, x31, 17
    jalr x0, x1, 0
