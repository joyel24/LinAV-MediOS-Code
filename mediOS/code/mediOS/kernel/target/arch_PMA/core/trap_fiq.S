.section ".core"

@.align 5
 .global trap_fiq

 trap_fiq:

     ldr sp,=(trap_stack+0x400)
     stmfd  sp!,{lr}
     ldr    lr, __trap_fiq_ptr
     bx     lr
     ldmfd  sp!,{lr}
     subs   pc, lr, #4

 @.align 5

 __trap_fiq_ptr:  .word do_trap_fiq