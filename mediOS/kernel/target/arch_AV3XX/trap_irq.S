irq_reg0 = 0x30504
irq_reg1 = 0x30506



.section ".core"  

.align	5
.global __irq_trap
__irq_trap:
    sub     sp, sp, #72
    stmia   sp, {r0 - r12}			@ save r0 - r12
    ldr     r7, .LCirq
    add     r5, sp, #72
    ldmia   r7, {r7 - r9}
    add     r4, sp, #52
    mov     r6, lr
    stmia   r4, {r5, r6, r7, r8, r9}	@ save sp_SVC, lr_SVC, pc, cpsr, old_ro
@ now we are set, we can search which irq we have to handle
@     ldr     r0, =irq_reg1    
@     ldrh    r0, [r0]
@     mov     r0, r0, lsl  #16
@     ldr     r6, =irq_reg0               @IRQ0_STATUS reg addr
@     ldrh    r6, [r6]                    @ get interrupts irq_0 and irq_1
@     
@     orr     r6, r6, r0
@     mvn     r6, r6
@     mov     r0, #0
@ irqsearch1:
@     tst     r6, #1                       @ Is IRQ set?
@     bne     irqsearch2                   @ Yes... exit with NEQ
@     add     r0, r0, #1
@     mov     r6, r6, lsr #1
@     cmp     r0, #32
@     bcc     irqsearch1
@ irqsearch2:                                          @ EQ will be set if we reach 32                
@     movne   r1, sp
@ /*    
@     routine called with r0 = irq number, r1 = struct pt_regs *
@     */
@     adrne   lr, __irq_svc1
@     bne     do_IRQ

    mov     r0, #0
    mov     r1, sp
    adr     lr, __irq_svc1
    b       irq_globalHandler
__irq_svc1:
    ldr     r0, [sp, #64]		@ irqs are already disabled
    msr     spsr, r0
    ldmia   sp, {r0 - pc}^			@ load r0 - pc, cpsr

.align	5
.LCirq:		.word	__temp_irq                
@__irq_mask:     .word   
