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
    mcr	p15,0,r0,c8,c5,0 /* invalidate TLBs */
    mcr	p15,0,r0,c8,c6,0 /* invalidate TLBs */
    mcr	p15,0,r0,c8,c7,0 /* invalidate TLBs */

    nop
    nop
    nop
    nop

    /* disable TCMs (needed, so they don't conflict with cache for iram) */
    mov r0,#0
    mcr p15,0,r0,c9,c1,0
    mcr p15,0,r0,c9,c1,1

    /* configure mmu so iram is cached (archos sucks) */
      /* setup a second level descriptor for range 0-1M */
    ldr r0,=0x18fb000 /* my coarse table base address */
    ldr r1,=0x0ff1    /* = coarse table, uncached unbuffered, full access */
    ldr r2,=0x0ffd    /* = coarse table, cached buffered, full access */
    mov r6,#0

    mov r3,#16
    mmu_loop:
        mov r4,#16
        mmu_loop2:
            cmp r3,#16
            orreq r5,r6,r2
            orrne r5,r6,r1
            str r5,[r0],#4
            subs r4,r4,#1
            bne mmu_loop2
        add r6,r6,#0x10000
        subs r3,r3,#1
        bne mmu_loop

      /* update first level descriptor */
    ldr r0,=0x18fc000
    ldr r1,=0x18fb011 /* = coarse table, located at 0x18fb000 */
    str r1,[r0]

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

