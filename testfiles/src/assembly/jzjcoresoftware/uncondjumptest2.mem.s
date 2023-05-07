    addi x31, x0, 2
    jalr x31, x0, 0
    addi x31, x31, 2
    .insn r CUSTOM_0, 0, 0, x0, x0, x0#Exit
