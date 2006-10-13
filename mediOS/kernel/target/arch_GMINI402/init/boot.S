/*
*   kernel/target/arch/boot.S
*   Gmini402 boot
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/
.section ".text.init.arch"
.global entry_code
entry_code:
    /* get starting address */
    mov r7,pc
    sub r7,r7,#8
    
    /* set UART0 to 115200 bauds */
    ldr r5,=0x30302
    mov r1,#0x36
    strh r1,[r5]

    /* Disable IRQ+FIQ and set SYS mode */
    msr cpsr_c, #0xDF
    
    /* disable caches & mmu */
    mrc p15,0,r0,c1,c0,0
    ldr r1,=0xFFFFEFF8
    and r0,r0,r1
    mcr p15,0,r0,c1,c0,0

    nop
    nop
    nop
    nop

    /* invalidate caches */
    mov r0,#0x0
    mcr p15,0,r0,c7,c7,0 /* invalidate caches */
    mcr	p15,0,r0,c7,c10,4 /* drain WB */
    mcr	p15,0,r0,c8,c7,0 /* invalidate TLBs */

    nop
    nop
    nop
    nop

    /* re-enable caches & mmu */
    mrc p15,0,r0,c1,c0,0
    ldr r1,=0x00001805
    orr r0,r0,r1
    mcr p15,0,r0,c1,c0,0

    /* copy everything to the sdram start */
    mov r3,#0x20
    ldr r4,=0x30500

    mov r0, r7
    ldr r1, __code_start_ptr

    /* don't copy the .bss part */
    ldr r2, __real_iram_start_ptr
    ldr r5, __iram_end_ptr
    add r2,r2,r5
    ldr r5, __iram_start_ptr
    sub r2,r2,r5
    ldr r5, __sdend_end_ptr
    add r2,r2,r5
    ldr r5, __sdend_start_ptr
    sub r2,r2,r5

    sub r2, r2, r1
    mv_sdram_loop:
        /* handle watchdog */
        tst r2,#0x40000 /* change watchdog bit state each 256KB */
        streqh r3,[r4,#0x92]
        strneh r3,[r4,#0x8c]
        /* copy data (256b at a time = sdram line size) */
        ldmia r0!,{r5,r6,r7,r8,r9,r10,r11,r12}
        stmia r1!,{r5,r6,r7,r8,r9,r10,r11,r12}
        subs r2, r2, #32
        bhi mv_sdram_loop

    b kernel_entry

__real_iram_start_ptr: .word _real_iram_start
__iram_start_ptr:      .word _iram_start
__iram_end_ptr:        .word _iram_end
__sdend_start_ptr:     .word _sdend_start
__sdend_end_ptr:       .word _sdend_end
__code_start_ptr:      .word _code_start

