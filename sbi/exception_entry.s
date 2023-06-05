# exception_entry.s
# Copyright (C) 2023 John Jekel and Nick Chan
# See the LICENSE file at the root of the project for licensing info.
#
# Entry point back into M mode upon recieving an exception
#

.global __riscv_synchronous_exception_and_user_mode_swi_handler
__riscv_synchronous_exception_and_user_mode_swi_handler:
    #Restore the M-mode stack pointer and save the S-mode one (swap with mscratch)
    csrrw sp, mscratch, sp

    #Push all registers onto the stack (full descending), with the following exceptions:
    #sp: Since it is already saved in mscratch
    #a0, a1: Since they may be clobbered according to the RISC-V SBI (and also may be needed to return)
    #FIXME: IF IT IS NOT A SUPERVISOR ECALL, THEN PRESERVE A0 AND A1 TOO
    addi sp, sp, 31*-4#Make room
    sw x1, 0(sp)
    #sw x2, 4(sp)#sp
    sw x3, 8(sp)
    sw x4, 12(sp)
    sw x5, 16(sp)
    sw x6, 20(sp)
    sw x7, 24(sp)
    sw x8, 28(sp)
    sw x9, 32(sp)
    #sw x10, 36(sp)#a0
    #sw x11, 40(sp)#a1
    sw x12, 44(sp)
    sw x13, 48(sp)
    sw x14, 52(sp)
    sw x15, 56(sp)
    sw x16, 60(sp)
    sw x17, 64(sp)
    sw x18, 68(sp)
    sw x19, 72(sp)
    sw x20, 76(sp)
    sw x21, 80(sp)
    sw x22, 84(sp)
    sw x23, 88(sp)
    sw x24, 92(sp)
    sw x25, 96(sp)
    sw x26, 100(sp)
    sw x27, 104(sp)
    sw x28, 108(sp)
    sw x29, 112(sp)
    sw x30, 116(sp)
    sw x31, 120(sp)
    #TODO

    #TODO decide what to do based on the kind of exception this was
    #Ex. if it was a supervisor ECALL, call handle_smode_ecall
    #FIXME: IF IT IS NOT A SUPERVISOR ECALL, THEN PRESERVE A0 AND A1 TOO
    #FIXME don't just assume this is an ecall
    call handle_smode_ecall#Arguments are already in a0 thru a7

    #Pop registers from the stack (full descending), with the same exceptions as before
    lw x1, 0(sp)
    #lw x2, 4(sp)#sp
    lw x3, 8(sp)
    lw x4, 12(sp)
    lw x5, 16(sp)
    lw x6, 20(sp)
    lw x7, 24(sp)
    lw x8, 28(sp)
    lw x9, 32(sp)
    #lw x10, 36(sp)#a0
    #lw x11, 40(sp)#a1
    lw x12, 44(sp)
    lw x13, 48(sp)
    lw x14, 52(sp)
    lw x15, 56(sp)
    lw x16, 60(sp)
    lw x17, 64(sp)
    lw x18, 68(sp)
    lw x19, 72(sp)
    lw x20, 76(sp)
    lw x21, 80(sp)
    lw x22, 84(sp)
    lw x23, 88(sp)
    lw x24, 92(sp)
    lw x25, 96(sp)
    lw x26, 100(sp)
    lw x27, 104(sp)
    lw x28, 108(sp)
    lw x29, 112(sp)
    lw x30, 116(sp)
    lw x31, 120(sp)
    addi sp, sp, 31*4#Remove room on the stack

    #Restore the S-mode stack pointer and save the M-mode one (swap with mscratch)
    csrrw sp, mscratch, sp

    #Return from the exception
    mret
