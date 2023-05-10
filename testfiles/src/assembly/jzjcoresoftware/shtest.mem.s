    sh x0, 16(x0)
    lhu x31, 16(x0)
    lhu x31, 18(x0)
    .insn r CUSTOM_0, 0, 0, zero, zero, zero#Exit

    .word 0x89abcdef
    #The assembler will write the above data into memory as 0xefcdab89 starting at address 0x16
    #sh will overwrite part of the data and turn it to 0x0000ab89
    #x31 will then be loaded with 0x00000000 followed by 0x000089ab
