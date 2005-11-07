.section ".vectors"
    b trap_undef
    b trap_swi
    b trap_prefetch
    b trap_data
    b trap_addrexcptn
    b trap_irq
    b trap_fiq

.section ".core"    

.align 5
.global trap_undef
trap_undef:
    stmfd  sp!,{lr}
    ldr    lr, __trap_undef_ptr
    bx     lr
    ldmfd  sp!,{lr}
    movs   pc, lr    
.align 5
__trap_undef_ptr:  .word do_trap_undef

.align	5
.global trap_swi
trap_swi:
   @// Determine SWI code...
   stmdb sp!, {r0}
   ldr r0,[lr,#-4]
   bic r0,r0,#0xff000000

   @// Check for "yield" command...
   @removed yielding
   @cmp r0, #0
   @beq yielding

                    @// r0 contains command code...
   ldmia sp!, {r3}  @// r3 contains first parameter...
   mov r5, lr       @// r5 contains user mode return address...
   mrs r4, spsr     @// r4 contains user state...
   msr cpsr_all, r4 @// switching to user mode...

@// HERE INTERRUPTS ALREADY ENABLED TO GET API CALL REENTRANCY

   stmdb sp!, {r5, lr}
   bl kcswi_handler
   ldmia sp!, {r5, lr}
   mov pc, r5       @// return to user code...


@--------------------------------------------------------------
@// Previous handler:
@    stmfd  sp!,{lr}
@    ldr    lr, __trap_swi_ptr
@    bx     lr
@    ldmfd  sp!,{lr}
@    movs   pc, lr
.align 5
__trap_swi_ptr:  .word do_trap_swi    
    
.align	5
.global trap_prefetch
trap_prefetch:
    stmfd  sp!,{lr}
    ldr    lr, __trap_prefetch_ptr
    bx     lr
    ldmfd  sp!,{lr}
    subs   pc, lr, #4    
.align 5
__trap_prefetch_ptr:  .word do_trap_prefetch
        
.align	5
.global trap_data
trap_data:
    stmfd  sp!,{lr}
    ldr    lr, __trap_data_ptr
    bx     lr
    ldmfd  sp!,{lr}
    subs   pc, lr, #8    
.align 5
__trap_data_ptr:  .word do_trap_data
        
.align	5
.global trap_addrexcptn
trap_addrexcptn:
    stmfd  sp!,{lr}
    ldr    lr, __trap_addrexcptn_ptr
    bx     lr
trap_addrexcptn_loop:
    b      trap_addrexcptn_loop    
.align 5
__trap_addrexcptn_ptr:  .word do_trap_addrexcptn

.align	8
.global trap_irq
trap_irq:
    @
    @ save mode specific registers
    @
    ldr    r13, .LCsirq
    sub    lr, lr, #4
    str    lr, [r13]                @ save lr_IRQ
    mrs    lr, spsr
    str    lr, [r13, #4]            @ save spsr_IRQ
    @
    @ now branch to the relevent MODE handling routine
    @
    mrs    r13, spsr            @ switch to correct mode
    bic    r13, r13, #0x1f      @ preserve F and T bits
    orr    r13, r13, #0x13      @ 0x1F for SYS_32,   0x13 for SVC_32
    orr    r13, r13, #0x80
    msr    spsr_c, r13          @ switch to correct mode
    
    @and    lr, lr, #15
    ldr    lr, [pc]
    movs   pc, lr                @ Changes mode and branches
__trap_irq_ptr:  .word __irq_trap

.align 5
.LCsirq:	 .word __temp_irq

.ifdef AV3XX
.include "av_trap_fiq.S"
irq_reg0 = 0x30504
irq_reg1 = 0x30506
.else
.ifdef GMINI4XX
.include "g4xx_trap_fiq.S"
irq_reg0 = 0x30508
irq_reg1 = 0x3050A
irq_reg2 = 0x3050C
.else
.print "Error undef ARCH"
.endif
.endif


.align	5
__irq_trap:
    sub     sp, sp, #72
    stmia   sp, {r0 - r12}			@ save r0 - r12
    ldr     r7, .LCirq
    add     r5, sp, #72
    ldmia   r7, {r7 - r9}
    add     r4, sp, #52
    mov     r6, lr
    stmia   r4, {r5, r6, r7, r8, r9}	@ save sp_SVC, lr_SVC, pc, cpsr, old_ro
    ldr     r0, =irq_reg1    
    ldrh    r0, [r0]
    mov     r0, r0, lsl  #16
    ldr     r6, =irq_reg0  @IRQ0_STATUS reg addr
    ldrh    r6, [r6]               @ get interrupts irq_0 and irq_1
    
    orr     r6, r6, r0
    mvn     r6, r6
    mov     r0, #0
irqsearch1:
    tst     r6, #1                       @ Is IRQ set?
    bne     irqsearch2                   @ Yes... exit with NEQ
    add     r0, r0, #1
    mov     r6, r6, lsr #1
    cmp     r0, #32
    bcc     irqsearch1
irqsearch2:                                          @ EQ will be set if we reach 32                
    movne   r1, sp
    @
    @ routine called with r0 = irq number, r1 = struct pt_regs *
    @
    adrne   lr, __irq_svc1
    bne     do_IRQ
__irq_svc1:
@specific irq code for gmini
.ifdef GMINI4XX
    ldr     r6, =irq_reg2          @IRQ2_STATUS reg addr
    ldrh    r6, [r6]               
irqsearch1_G:
    tst     r6, #1                       @ Is IRQ set?
    bne     irqsearch2_G                   @ Yes... exit with NEQ
    add     r0, r0, #1
    mov     r6, r6, lsr #1
    cmp     r0, #48
    bcc     irqsearch1_G
irqsearch2_G:                                          @ EQ will be set if we reach 48                
    movne   r1, sp
    @
    @ routine called with r0 = irq number, r1 = struct pt_regs *
    @
    adrne   lr, __irq_svc1_G
    bne     do_IRQ
__irq_svc1_G:   
.endif    
    ldr     r0, [sp, #64]		@ irqs are already disabled
    msr     spsr, r0
    ldmia   sp, {r0 - pc}^			@ load r0 - pc, cpsr

.align	5
.LCirq:		.word	__temp_irq                

__temp_irq:	.word	0				@ saved lr_irq
		.word	0				@ saved spsr_irq
		.word	-1				@ old_r0
