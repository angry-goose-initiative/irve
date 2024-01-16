# exception_entry.s
# Copyright (C) 2023-2024 John Jekel
# See the LICENSE file at the root of the project for licensing info.
#
# Entry point back into M mode upon recieving an exception
#

#TODO support DIRECT mode too (not just vectored)

.global ___rvsw_exception_handler___
___rvsw_exception_handler___:
    #Restore the M-mode stack pointer and save the S-mode one (swap with mscratch)
    csrrw sp, mscratch, sp

    #Push all S-Mode registers onto the stack (full descending), except for sp since it is already saved in mscratch
    addi sp, sp, 31 * -4#Make room on the stack
    sw x1, 0(sp)
    sw zero, 4(sp)#sw x2, 4(sp)#Skip sp, and instead push a nonsensical 0 to aid in debugging
    sw x3, 8(sp)
    sw x4, 12(sp)
    sw x5, 16(sp)
    sw x6, 20(sp)
    sw x7, 24(sp)
    sw x8, 28(sp)
    sw x9, 32(sp)
    sw x10, 36(sp)
    sw x11, 40(sp)
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

    #Now we can clobber registers and still get S-Mode ones back later
    #If we actually want to update S-Mode registers, we have to write to the stack
    #Then the new values will be incorperated when we restore them from the stack later

    #Restore the M-Mode global and thread pointers so C and assembly code can access global variables
    call restore_mmode_gp_tp

    #There are no other M-Mode registers to restore, since this is an entry back into M-mode.
    #See jump2linux.S for more info

    #Read mcause into t0. If it is 9 (an S-Mode ECALL), then jump to is_smode_ecall
    csrr t0, mcause
    li t1, 9
    beq t0, t1, is_smode_ecall

isnt_smode_ecall:
    #The function will have to get the registers it needs from the stack, and update the values on the stack which we will restore later
    #Ex. we could have to emulate an instruction in this case
    #We thus copy the S-Mode in mscratch currently into the stack at where sp would have otherwise been
    #We will then copy this back from the stack into mscratch before jumping to return_from_exception
    #Then we can just pass a pointer to the registers on the stack to the function, and use the normal calling convention!

    csrr t0, mscratch#t0 contains the S-Mode SP
    sw t0, 4(sp)#Store the S-Mode SP on the stack, at where sp would have otherwise been

    #Setup and execute the call
    mv a0, sp#Pass a pointer to the S-Mode registers on the stack (which is just the M-Mode stack pointer)
    csrr a1, mcause#Pass the mcause as well for convenience
    csrr a2, mepc#Pass the mepc as well for convenience
    call handle_other_exceptions#void handle_other_exceptions(uint32_t registers[31], uint32_t mcause, uint32_t mepc);

    #Move the potentially updated S-Mode SP on the stack back into mscratch
    lw t0, 4(sp)#Load the (potentially updated) S-Mode SP from the stack, at where sp would have otherwise been
    csrw mscratch, t0#Store the (potentially updated) S-Mode SP back mscratch as the return_from_exception code expects

    j return_from_exception

is_smode_ecall:
    #SBI arguments are already in S-Mode registers a0 thru a7, which are currently on the stack
    #However, they're also still in the M-Mode registers a0 thru a7, and we haven't clobbered them
    #So we can avoid a copy here and use the standard calling convention (since the SBI call info should be entirely contained in a0 thru a7)

    #We have to decide if this is a legacy or new SBI call in order to deal with whether a1 will be clobbered or not
    #Every EID less that 0x10 (exclusive) is a legacy SBI call
    li t0, 0x00000010
    bgeu a7, t0, new_sbi
    
legacy_sbi:
    call handle_legacy_sbi_smode_ecall
    j wrapup_smode_ecall
    #The result is in M-Mode register a0. However, we need to update the S-Mode register a0
    #It is currently stored on the stack. So copy the new a0 there
    #In this way we give return_from_exception what it expects
    #sw a0, 36(sp)#Handled in wrapup_smode_ecall

new_sbi:
    call handle_sbi_smode_ecall
    #The result is in M-Mode registers a0 and a1. However, we need to update the S-Mode registers a0 and a1
    #These are currently stored on the stack. So copy the new a0 and a1 to the proper places on the stack
    #In this way we give return_from_exception what it expects
    #sw a0, 36(sp)#Handled in wrapup_smode_ecall
    sw a1, 40(sp)

wrapup_smode_ecall:
    #Both legacy and new SBI calls update the S-Mode register a0
    sw a0, 36(sp)
    #Also increment the S-Mode PC by 4 to skip past the ECALL instruction
    csrr t0, mepc
    addi t0, t0, 4
    csrw mepc, t0

return_from_exception:
    #Preserve the M-Mode global and thread pointers again
    call preserve_mmode_gp_tp

    #Pop all S-Mode registers from the stack (full descending), except for sp since it is saved in mscratch
    lw x1, 0(sp)
    #lw x2, 4(sp)#Skip sp
    lw x3, 8(sp)
    lw x4, 12(sp)
    lw x5, 16(sp)
    lw x6, 20(sp)
    lw x7, 24(sp)
    lw x8, 28(sp)
    lw x9, 32(sp)
    lw x10, 36(sp)
    lw x11, 40(sp)
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
    addi sp, sp, 31 * 4#Remove room on the stack

    #Restore the S-mode stack pointer and save the M-mode one (swap with mscratch)
    csrrw sp, mscratch, sp

    #Return from the exception
    mret
