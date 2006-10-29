/*
*   kernel/boot.S
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/
.section ".text.init"
.global kernel_entry
kernel_entry:
/* initialize the stacks and disable irq+fiq */
    MSR CPSR_c, #0xD3 @0x80 | 0x40 | 0x13 = 0xD3 [SVC mode, used in SWI and IRQ]
    @mov r0, #0x0000000
	ldr r0, =_svc_IniStack
	ldr r0,[r0]
    mov sp, r0
    MSR CPSR_c, #0xDF @0x80 | 0x40 | 0x1F = 0xDF [SYS mode]
	ldr r0, =_sys_IniStack
	ldr r0,[r0]
       
    mov sp, r0
/* init lock var for power off */
    ldr r0, =powering_off
    mov r1, #0
    str r1, [r0]
/* move iram code */
    ldr r0, __real_iram_start_ptr
    ldr r1, __iram_start_ptr
    ldr r2, __iram_end_ptr
    sub r2, r2, r1
    cmp r2, #0
    beq no_mv_iram
mv_iram_loop:
    ldr     r3, [r0],#4
    str     r3, [r1],#4
    subs    r2, r2, #4
    bne     mv_iram_loop
no_mv_iram:
/* move sdend code */
    @ after the prev loop, R0 contains the real iram end address, which is also the real sdend start
    ldr r1, __sdend_start_ptr
    ldr r2, __sdend_end_ptr
    sub r2, r2, r1
    cmp r2, #0
    beq no_mv_sdend
mv_sdend_loop:
    ldr     r3, [r0],#4
    str     r3, [r1],#4
    subs    r2, r2, #4
    bne     mv_sdend_loop
no_mv_sdend:
/* enable fiq for wdt */
    mrs      r0, cpsr
    bic      r0, r0, #0x40
    msr      cpsr_c, r0
/* clear bss */
    ldr     r1, __bss_start_ptr
    ldr     r2, __bss_end_ptr
    sub     r2, r2, r1
    cmp r2, #0
    beq no_clr_bss
    mov     r3, #0
clr_bss_loop:
    str     r3,[r1],#4
    subs    r2, r2, #4
    bne     clr_bss_loop
no_clr_bss:
/* launch start_function */
    ldr    r3, __kernel_start_ptr
    mov    lr,pc
    bx     r3

/* loop endlessly */
infinite_loop:
    b infinite_loop

__kernel_start_ptr:    .word kernel_start
__real_iram_start_ptr: .word _real_iram_start
__iram_start_ptr:      .word _iram_start
__iram_end_ptr:        .word _iram_end
__sdend_start_ptr:     .word _sdend_start
__sdend_end_ptr:       .word _sdend_end
__bss_start_ptr:       .word _bss_start
__bss_end_ptr:         .word _bss_end
