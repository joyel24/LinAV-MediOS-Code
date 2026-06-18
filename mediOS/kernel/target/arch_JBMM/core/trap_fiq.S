.section ".core"  

.align	5
.global trap_fiq
trap_fiq:
    ldr     r8, =0x30600
    ldrh    r9, [r8,#0xa]
    tst     r9, #0x100
    bne     iniAvLoopVal2
    ldr     r9, =powering_off
    ldr     r8, [r9]
    mov     r9, #1
    cmp     r9,r8
    beq     iniAvLoopVal2
    ldr     r9, =.avLOOPval2
    ldr     r8, [r9]
    add     r8, r8, #1
    str     r8, [r9]
    ldr     r9, =0x9c4 @1388
    cmp     r8, r9
    ble     normalFIQ
    mov     r8, #0
    ldr     r9, =0x305a0
    ldr     r10,=0x30a00
    strh    r8, [r9]
    strh    r8, [r10]
    subs    pc, lr, #4    
iniAvLoopVal2:
    mov     R8, #0
    ldr     R9, =.avLOOPval2
    str     r8, [r9]
normalFIQ:
    ldr     r8, =.avLOOPval1
    ldr     r9,[r8]
    add     r9,r9,#1
    str     r9,[r8]
    and     r9, r9, #1
    ldr     r10,=0x30a00
    strh    r9, [r10]    
    mov     r8,#1
    ldr     r9, =0x30300
    ldr     r10,=0x30580
    strh    r8, [r9]
    strh    r8, [r10]
    subs    pc, lr, #4

.avLOOPval1:	.long	0x0
.avLOOPval2:	.long	0x0
.avLOOPval3:	.long	0x0
