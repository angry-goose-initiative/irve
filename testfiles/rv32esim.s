.global _start
_start:
.local string
.text
    nop
    li x1, 123456789
    li x2, 123
    addi x3, x2, 456
    slti x4, x3, 17
    sltiu x5, x3, 17
    lw x6, 0(x0)
    j loop_start

    #Print out hello world
loop_start:
    la x7, string
    #li x7, 0x40
loop:
    lbu x8, 0(x7)
    beqz x8, end
    sb x8, -1(x0)#Physical memory address 0xFFFFFFFF is the debug address
    addi x7, x7, 1
    j loop
end:
    ecall

.text
string:
.string "Hello World!\n"
