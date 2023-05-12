    addi x30, x0, 26
    sw x30, 12(x0) 
    lw x31, 12(x0)
    .insn r CUSTOM_0, 0, 0, zero, zero, zero#Exit
