/*
*   kernel/boot.S
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/
    .section ".text.init"
.global entry_code
entry_code:
/* initialize the stacks and disable irq+fiq */
/*    MSR CPSR_c, #0xD1 @0x80 | 0x40 | 0x11 = 0xD1 [FIQ mode]
    mov r0, #0x00000000 @ no stack for FIQ mode
    mov sp, r0
    MSR CPSR_c, #0xD2 @0x80 | 0x40 | 0x12 = 0xD2 [IRQ mode]
    mov r0, #0x00008000
    mov sp, r0*/
    MSR CPSR_c, #0xD3 @0x80 | 0x40 | 0x13 = 0xD3 [SVC mode, used in SWI and ]
    @mov r0, #0x00007F00
    mov r0, #0x00008000
    mov sp, r0
    MSR CPSR_c, #0xDF @0x80 | 0x40 | 0x1F = 0xDF [SYS mode]
    @mov r0, #0x00007C00
    mov r0, #0x00007D00
    mov sp, r0
/* move iram code */
    ldr r0, __real_iram_start_ptr    
    ldr r1, __iram_start_ptr
    ldr r2, __iram_end_ptr
    sub r2, r2, r1
mv_iram_loop:
    ldr     r3, [r0],#4
    str     r3, [r1],#4
    subs    r2, r2, #4
    bne     mv_iram_loop
/* enable fiq for wdt */
    mrs      r0, cpsr
    bic      r0, r0, #0x40
    msr      cpsr_c, r0
/* clear bss */
    ldr     r1, __bss_start_ptr
    ldr     r2, __bss_end_ptr
    sub     r2, r2, r1
    mov     r3, #0
clr_bss_loop:
    str     r3,[r1],#4
    subs    r2, r2, #4
    bne     clr_bss_loop
/* launch start_function */
    ldr    r3, __kernel_start_ptr
    mov    lr,pc
    bx     r3

/* Gmini400: reload the firmware */
.ifdef GMINI4XX
    ldr r0,=0x900000
    bx r0
.endif

__kernel_start_ptr:    .word kernel_start
__real_iram_start_ptr: .word _real_iram_start
__iram_start_ptr:      .word _iram_start
__iram_end_ptr:        .word _iram_end
__bss_start_ptr:       .word _bss_start
__bss_end_ptr:         .word _bss_end
