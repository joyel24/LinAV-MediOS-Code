.section .core
.global reset_device
reset_device:
    MRS R0, CPSR
    BIC R0, R0, #0x1F
    ORR R0, R0, #0xD3
    MSR CPSR_cf, R0
    ldr r0,=0x30920    
    mov r1,#0x2F
    strh r1,[r0]
    LDR R0,=0x100004
    MOV LR, PC
    MOV PC, R0
    