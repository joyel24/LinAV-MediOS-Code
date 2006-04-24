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
    @mov     r0, #0
    @mov     r1, sp
    mov     r0, sp
    adr     lr, __irq_svc1
    b       irq_globalHandler
__irq_svc1:
    ldr     r0, [sp, #64]		@ irqs are already disabled
    msr     spsr, r0
    ldmia   sp, {r0 - pc}^			@ load r0 - pc, cpsr

.align	5
.LCirq:		.word	__temp_irq                
@__irq_mask:     .word   
