.section ".vectors"
    b trap_undef
    b trap_swi
    b trap_prefetch
    b trap_data
    b trap_addrexcptn
    b trap_irq
    b trap_fiq

.section ".core"    

@.align 5
.global trap_undef
trap_undef:
    ldr sp,=(trap_stack+0x400)
    stmfd  sp!,{lr}
    ldr    lr, __trap_undef_ptr
    bx     lr
    ldmfd  sp!,{lr}
    movs   pc, lr
@.align 5
__trap_undef_ptr:  .word do_trap_undef

@.align	5
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
   ldr r5, __trap_swi_ptr
   mov lr, pc
   bx  r5
 @   mov pc, r5
   @bl kcswi_handler
   ldmia sp!, {r5, lr}
@   bx  r5
   mov pc, r5       @// return to user code...


@--------------------------------------------------------------
@// Previous handler:
@    stmfd  sp!,{lr}
@    ldr    lr, __trap_swi_ptr
@    bx     lr
@    ldmfd  sp!,{lr}
@    movs   pc, lr
@.align 5
__trap_swi_ptr:  .word swi_handler

@.align	5
.global trap_prefetch
trap_prefetch:
    ldr sp,=(trap_stack+0x400)
    stmfd  sp!,{lr}
    ldr    lr, __trap_prefetch_ptr
    bx     lr
    ldmfd  sp!,{lr}
    subs   pc, lr, #4
@.align 5
__trap_prefetch_ptr:  .word do_trap_prefetch

@.align	5
.global trap_data
trap_data:
    ldr sp,=(trap_stack+0x400)
    stmfd  sp!,{lr}
    ldr    lr, __trap_data_ptr
    bx     lr
    ldmfd  sp!,{lr}
    subs   pc, lr, #8
@.align 5
__trap_data_ptr:  .word do_trap_data

@.align	5
.global trap_addrexcptn
trap_addrexcptn:
    ldr sp,=(trap_stack+0x400)
    stmfd  sp!,{lr}
    ldr    lr, __trap_addrexcptn_ptr
    bx     lr
trap_addrexcptn_loop:
    b      trap_addrexcptn_loop
@.align 5
__trap_addrexcptn_ptr:  .word do_trap_addrexcptn

.include "../../include/kernel/target/arch/trap_irq.inc"

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
    mrs    r13, cpsr            @ switch to correct mode
    bic    r13, r13, #0x3f      @ preserve F bits (VP do not preserve T bit ! => 3F)
    orr    r13, r13, #0x13      @ 0x1F for SYS_32,   0x13 for SVC_32
    orr    r13, r13, #0x80
    msr    cpsr_c, r13          @ switch to correct mode
    @
    @ now real irq handler
    @
    sub     sp, sp, #72
    stmia   sp, {r0 - r12}			@ save r0 - r12
    ldr     r7, .LCsirq
    add     r5, sp, #72
    ldmia   r7, {r7 - r9}
    add     r4, sp, #52
    mov     r6, lr
    stmia   r4, {r5, r6, r7, r8, r9}	@ save sp_SVC, lr_SVC, pc, cpsr, old_ro
__irq_loop:
    ldr    R2, =irq_entry
    ldrh   R3, [R2,#2]
    ldrh   R2, [R2]
    orr    R4, R2, R3, LSL #16
    @ R2 points on the bloc  
    @ read whole bloc
    ldmia  R4, {R0-R3}
    cmp    R3, #0
    beq    __exit_irq_loop   @ if handler = 0 => no handler
    @ now ack the INT
    strh   R1, [R2]   
    mov    R1, SP            @ get reg stucture
    adr    LR, __irq_loop
    mov    PC, R3            @ branch to handler 
__exit_irq_loop:
    ldr     r0, [sp, #64]    @ irqs are already disabled
    msr     spsr, r0
    ldmia   sp, {r0 - pc}^   @ load r0 - pc, cpsr

@.align 5
.LCsirq:	 .word __temp_irq

.global __temp_irq
__temp_irq: .word	0  @ saved lr_irq
            .word	0  @ saved spsr_irq
            .word	-1 @ old_r0

               
.align  8
irq_fct_table:  
.space (nb_irq+1)*16 
.align
.global irq_tbl_ptr
irq_tbl_ptr: .word irq_fct_table
