        .section ".text"
.global init_stack
init_stack:
    mov r1, lr
    MSR CPSR_c, #0xD1 @0x80 | 0x40 | 0x11 = 0xD1 [FIQ mode]
    mov r0, #0x00008000
    mov sp, r0
    MSR CPSR_c, #0xD2 @0x80 | 0x40 | 0x12 = 0xD2 [IRQ mode]
    mov r0, #0x00007F00
    mov sp, r0
    MSR CPSR_c, #0xD3 @0x80 | 0x40 | 0x13 = 0xD3 [SVC mode, used in SWI]
    mov r0, #0x00007E00
    mov sp, r0
    MSR CPSR_c, #0xDF @0x80 | 0x40 | 0x1F = 0xDF [SYS mode]
    mov r0, #0x00007D00
    mov sp, r0
    mov pc, r1 

.global get_sp
get_sp:
    mov r0, sp
    mov pc, lr
    