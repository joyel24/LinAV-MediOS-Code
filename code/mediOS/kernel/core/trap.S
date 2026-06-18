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
    stmdb  sp!, {r0-r3}
    @ Determine SWI code : R3 = swi code
    ldr    r3,[lr,#-4]
    bic    r3,r3,#0xff000000    
    mrs    r2, spsr              @ save thread CPSR
    mov    r1, lr                @ save where we came from
    cmp    r3, #2                @ SWI 1 special yield with no curThread sav
    beq    yielding
    @ switch to SYS mode in order to save thread regs
    msr    cpsr_c, #0xDF
       
    ldr    r0, threadCurrent
    add    r0, r0, #68
    stmdb  r0!, {r1}             @ PC
    stmdb  r0!, {r4-r12,sp,lr}   @ other regs    
    @ switching to SVC mode with FIQ
    msr    cpsr_c, #0x93    
    @ store remaining regs: R0, R1 and CPSR   
    mov    r4, r2
    mov    r5, r0                @ saving ptr on reg struct 
    mov    r6, r3                @ saving swi code
    ldmia  sp!, {r0-r3}    
    stmdb  r5!, {r0-r3}
    stmdb  r5!, {r4}    
    mov    r3,r6

    @// Check for "yield" command...
    cmp r3, #0
    beq yielding
.ifdef DSC21
    ldr r5, __trap_swi_ptr
    mov lr, pc
    bx  r5
.else
    bl swi_handler
.endif
    @custom loadContext: R0 changed to return val
    msr    cpsr_c, #0xD3
    ldr    r1, threadCurrent
    ldmia  r1!,{r2}
    msr    spsr, r2    @ put back spsr to correct value
    add    lr, r1, #60
    add    r1, r1, #4
    @ moving to SYS without INT
    msr    cpsr_c, #0xDF     
    ldmia  r1, {r1 - lr}          @ load r0 - lr
    @ going back to SVC without INT
    msr    cpsr_c, #0xD3
    ldmia  lr,{pc}^
yielding:    
    bl     thread_nxt
    b      thread_loadContext
   

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
    @ we need to use this small stack in order to save R0->R2
    @ we can't just change the mode like in swi, because we would loose
    @ spsr and lr !!!
    ldr    sp, .LCsirq
    stmia  sp, {r0-r2}
    mrs    r2, spsr 
    sub    r1, lr, #4            @ save where we came from
    @ switch to SYS mode in order to save thread regs
    msr    cpsr_c, #0x9F
       
    ldr    r0, threadCurrent
    add    r0, r0, #68
    stmdb  r0!, {r1}             @ PC
    stmdb  r0!, {r3-r12,sp,lr}   @ other regs    
    @ switching to SVC mode with FIQ
    msr    cpsr_c, #0x93    
    @ store remaining regs: R0, R1 and CPSR   
    mov    r4, r2
    mov    r5, r0                @ saving ptr on reg struct 
    ldr    r3, .LCsirq           @ r3 points on saved R0, R1        
    ldmia  r3, {r0-r2}    
    stmdb  r5!, {r0,r1,r2}
    stmdb  r5!, {r4}
    @ r5 points on reg struct
    add    r5,r2,#8
    
__irq_loop:
.ifdef DSC21
    ldr    R2, =0x305c0
    ldrh   R3, [R2]
    mov    r2, r3, lsr #8
    and    r2, r2, #0x3F
    cmp    r2, #0
    beq    __exit_irq_loop
    and    r3, r3, #0x1F
    add    r3, r3, #1
    ldr    r2, irq_tbl_ptr
    add    r4, r2, r3, lsl #4
    ldmia  R4, {R0-R3}
.else
 .ifdef DSC25
  .equ IRQ_DSC25_COMPATIBLE,1
 .endif
 .ifdef DM270
  .equ IRQ_DSC25_COMPATIBLE,1
 .endif
 .ifdef DM320
  .equ IRQ_DSC25_COMPATIBLE,1
 .endif

 .ifdef IRQ_DSC25_COMPATIBLE
    ldr    R2, =irq_entry
    ldrh   R3, [R2,#2]
    ldrh   R2, [R2]
    orr    R4, R2, R3, LSL #16
    ldmia  R4, {R0-R3}
    cmp    R3, #0
    beq    __exit_irq_loop   @ if handler = 0 => no handler
 .else
  .err
 .endif
.endif
    @ R4 points on the bloc  
    @ read whole bloc

    @ now ack the INT
    strh   R1, [R2]   
    mov    R1, r5            @ get reg stucture
    adr    LR, __irq_loop    
    mov    PC, R3            @ branch to handler 
__exit_irq_loop:
    
    @ no need to do it here as we switch only on tick INT
    @bl     thread_nxt

.global thread_loadContext
thread_loadContext:
@ going to SVC mode without INT
    
    msr    cpsr_c, #0x93
    ldr    r0, threadCurrent
    ldmia  r0!,{r1}
    msr    spsr, r1    @ put back spsr to correct value
    add    lr, r0, #60
    @ moving to SYS without INT
    msr    cpsr_c, #0x9F     
    ldmia  r0, {r0 - lr}          @ load r0 - lr
    @ going back to SVC without INT
    msr    cpsr_c, #0x93
    ldmia  lr,{pc}^
    

.align 5
.LCsirq:	 .word __temp_irq

.global __temp_irq
__temp_irq: .long	0  @ saved lr_irq
            .long	0  @ saved spsr_irq
            .long	-1 @ old_r0
            
.align 5
.global threadCurrent
threadCurrent: .byte 0, 0, 0, 0

               
.align  8
irq_fct_table:  
.space (nb_irq+1)*16 
.align
.global irq_tbl_ptr
irq_tbl_ptr: .word irq_fct_table
