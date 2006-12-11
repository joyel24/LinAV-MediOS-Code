/*
*   kernel/target/arch/boot.S
*   Av1XX boot
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/
.section ".text.init.arch"
.global entry_code
entry_code:

   /* av4 init */
    mov r7,pc
    sub r7,r7,#8

    /* copy everything to the sdram start */
    msr cpsr_c, #0xDF /* Disable IRQ+FIQ and set SYS mode */

    /* save IARM data */
    /* ldr r1,=0x18C0000 
    mov r1,#0
    mov r2,#1
    add r1, r1, r2, lsl #24
    mov r2,#8
    add r1, r1, r2, lsl #20
    mov r2,#0xc
    add r1, r1, r2, lsl #16
    mov r2,#0
    mov r0,#8
    add r2, r2, r0, lsl #12
    mov r0,#0

    backup_loop:
        ldmia r0!,{r4,r5,r6,r8,r9,r10,r11,r12}
        stmia r1!,{r4,r5,r6,r8,r9,r10,r11,r12}
        subs r2, r2, #32
        bhi backup_loop  */

    /* set UART0 to 115200 bauds */
    mov r5,#0
    mov r1,#3
    add r5, r5, r1, lsl #16
    add r5, r5, r1, lsl #8
    
    mov r1,#0x36
    strh r1,[r5,#2]
    mov r1,#'A'
    mov r2,#' '
    strh r1,[r5]
    strh r2,[r5]
    strh r1,[r5]
    strh r2,[r5]
    strh r1,[r5]
    strh r2,[r5]
    /*
    * cache init
    */
    @ldr r5,=0x30e00
    mov r1,#3
    mov r5,#0
    add r5,r5, r1, lsl #16
    mov r1,#0xe
    add r5,r5, r1, lsl #8
    @ r1 = 0x30e00
    /* disable cache */
    mov r1,#0x0
    strh r1,[r5]
    /* clear cache */
    mov r1,#0x2
    strh r1,[r5]
    cacheclear:
      ldrb r2,[r5]
      ands r2,r2,r1
      bne cacheclear
    /* re-enable cache */
    @ldr r1,=0x0101
    mov r1,#0
    mov r2,#1
    add r1,r1,r2,lsl #8
    add r1,r1,r2
    @@@@ r1 = 0x101
    strh r1,[r5]

    mov r3,#0x20

    @ldr r4,=0x30500
    mov r4,#0
    mov r1,#3
    add r4,r4,r1,lsl #16
    mov r1,#5
    add r4,r4,r1,lsl #8
    @ r4 = 0x30500
    
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
        tst r2,#0x10000 /* change watchdog bit state each 64KB */
        streqh r3,[r4,#0x92]
        strneh r3,[r4,#0x8c]
        /* copy data (256b at a time = sdram line size) */
        ldmia r0!,{r5,r6,r7,r8,r9,r10,r11,r12}
        stmia r1!,{r5,r6,r7,r8,r9,r10,r11,r12}
        subs r2, r2, #32
        bhi mv_sdram_loop

    ldr pc,=kernel_entry

__real_iram_start_ptr: .word _real_iram_start
__iram_start_ptr:      .word _iram_start
__iram_end_ptr:        .word _iram_end
__sdend_start_ptr:     .word _sdend_start
__sdend_end_ptr:       .word _sdend_end
__code_start_ptr:      .word _code_start
    