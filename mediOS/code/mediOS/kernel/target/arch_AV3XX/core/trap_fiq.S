.section ".core"  

.align	5
.global trap_fiq
trap_fiq:
    ldr     r8, =0x30502
    ldrh    r9, [r8]
    tst     r9, #0x800
    bne     avFIQout
    ldrh    r8, [r8,#0x24]
    tst     r8, #0x800
    beq     avFIQout		@ wrong FIQ or MASK => out
    ldr     r8, =0x3058A
    ldrh    r9, [r8]
    movs    r9, r9, lsr #0xD
    bcs     iniAvLoopVal2
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
    mov     r9, #1
    mov     r9, r9, lsl r8
    ldr     r8,=0x30526    @ interrupt mask 1
    ldrh    r10, [r8]
    bic     r10, r10, r9
    strh    r10, [r8]    
    mov     r8,#0
    ldr     r9,=0x30a1a
    strh    r8,[r9]
    b       endFiq
iniAvLoopVal2:
    mov     R8, #0
    ldr     R9, =.avLOOPval2
    str     r8, [r9]
normalFIQ:
    ldr     r8, =.avLOOPval1
    ldr     r9,[r8]
    add     r9,r9,#1
    str     r9,[r8]
    mov     r8,#1
    add     r8,r8,r9
    ldr     r9, =0x30a1a
    strh    r8, [r9]
    ldr     r9, =.avLOOPval3
    ldr     r8,[r9]
    add     r8,r8,#1
    str     r8,[r9]
    ldr     r9, =lcd_bright
    ldr     r9,[r9]
    cmp     r8, r9
    bhi     lcd_off1
    ldr     r9, =lcd_state
    ldr     r9,[r9]
    cmp     r9, #0
    beq     lcd_off
    mov     r8,#0x80
    ldr     r9, =0x3058A
    strh    r8,[r9,#4]
    b       endLcd   
lcd_off1:
    ldr     r9, =.avLOOPval3
    mov     r8,#0
    str     r8,[r9]
lcd_off:
    mov     r8,#0x80
    ldr     r9, =0x3058A
    strh    r8,[r9]
endLcd:
    mov     r8,#1
    ldr     r9,=0x30400
    str     r8,[r9]
endFiq:
	@ ack the FIQ
    mov     r9,	#0xb
    mov     r8, #1
    mov     r8, r8, lsl r9
    ldr     r9, =0x30502
    ldrh    r10, [r9]
    orr     r10, r10, r8
    strh    r10, [r9]
avFIQout:
    subs    pc, lr, #4

.avLOOPval1:	.long	0x0
.avLOOPval2:	.long	0x0
.avLOOPval3:	.long	0x0
