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
   cmp r0, #0
   beq yielding

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

   @// Yielding...
yielding:
   ldmia sp!, {r0}
   stmdb sp!, {r0, r1, r2} @// Store temp registers...
   mrs r1, cpsr            @// Remember current CPU state...
   mov r2, lr              @// Remember address we came from...
   orr r1, r1, #0xC0
   msr cpsr_c, r1          @// Disable all interrupts...

   mrs r0, spsr            @// Get user's process state word...
   orr r0, r0, #0xC0       @// Disable all interrupts in user's state...
   msr cpsr_c, r0          @// switch to user's mode...

   @// Saving context in user's stack
   @// Push order:
   @// pc, lr, r12, r11, ... (sp must not be stored !!!)
   stmdb sp!, {r2}         @// Store "came from" address as new pc...
   stmdb sp!, {r3-r12, lr}
   mov r12, sp             @// Now r12 is our user-mode stack pointer :)
   msr cpsr_c, r1          @// Switch back to SWI mode...

   @// Store user registers r0, r1, r2
   mrs r4, spsr            @// r4 = SPSR...
   ldmia sp!, {r0, r1, r2} @// Load temp user registers back...
   stmdb r12!, {r0, r1, r2}@// Store user registers in user stack...
   stmdb r12!, {r4}        @// Store user SPSR...
   ldr r6, g_pTaskRing     @// r1 contains now address of active TCB...
   str r12, [r6]           @// User's top of stack stored in TCB...

   bl kset_next_ready_task
   ldr r1, g_pTaskRing     @// r1 = address of new active TCB...
   ldr r12, [r1]           @// Get user's top of stack from TCB...
   ldmia r12!, {r4}        @// Get user SPSR...
   msr spsr_cfsx, r4
   msr cpsr_cfsx, r4       @// switch to user's mode...
   mov sp, r12
   ldmia sp!, {r0-r12, lr, pc}^
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
@	subs pc, r14, #4

	stmdb sp!, {r0, r1, r2} @// Store temp registers...
	mrs r1, cpsr            @// Remember current CPU state...
	orr r1, r1, #0xC0
	msr cpsr_c, r1          @// Disable all interrupts...
	mrs r0, spsr            @// Get user's process state word...
	subs r2, lr, #4         @// Remember address we came from...
	orr r0, r0, #0xC0       @// Disable all interrupts in user's state...
	bic r0, r0, #0x20       @// We want ARM mode...
	msr cpsr_c, r0          @// switch to user's mode...

	@// Saving context in user's stack
	@// Push order:
	@// pc, lr, r12, r11, ... (sp must not be stored !!!)
	stmdb sp!, {r2}         @// Store "came from" address as new pc...
	stmdb sp!, {r3-r12, lr}
	mov r12, sp             @// Now r12 is our user-mode stack pointer :)
	msr cpsr_c, r1          @// Switch back to IRQ mode...

	@// Store user registers r0, r1, r2
	mrs r4, spsr            @// r4 = SPSR...
	ldmia sp!, {r0, r1, r2} @// Load temp user registers back...
	stmdb r12!, {r0, r1, r2}@// Store user registers in user stack...
	stmdb r12!, {r4}        @// Store user SPSR...
	ldr r1, g_pTaskRing     @// r1 contains now address of active TCB...
	str r12, [r1]           @// User's top of stack stored in TCB...

	MSR CPSR_c, #0x92
	bl __irq_trap
	bl kset_next_ready_task

.global kload_context
kload_context:
	ldr r1, g_pTaskRing     @// r1 = address of new active TCB...
	ldr r12, [r1]           @// Get user's top of stack from TCB...
	ldmia r12!, {r4}        @// Get user SPSR...
	msr spsr_cfsx, r4
	bic r4, r4, #0x20       @// We want ARM mode...
	msr cpsr_cfsx, r4       @// switch to user's mode...
	mov sp, r12
	ldmia sp!, {r0-r12, lr, pc}
@	ldmia sp!, {r0-r12, lr, pc}^

.align 5
.global g_pTaskRing
g_pTaskRing:
	.byte 0, 0, 0, 0

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

.align	5
__irq_trap:
    ldr     r0, =0x30506    
    ldrh    r0, [r0]
    mov     r0, r0, lsl  #16
    ldr     r6, =0x30504  @IRQ0_STATUS reg addr
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

    bne     do_IRQ
    mov pc, lr
