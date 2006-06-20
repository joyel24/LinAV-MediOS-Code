/*
*   kernel/target/arch/boot.S
*   Gmini400 boot
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/
.section ".text.init.arch"
.global entry_code
entry_code:
    
    /* set UART0 to 115200 bauds */
    ldr r5,=0x30302
    mov r1,#0x36
    strh r1,[r5]

    /* disable caches & mmu */
    mrc p15,0,r0,c1,c0,0
    ldr r1,=0xFFFFEFF8
    and r0,r0,r1
    mcr p15,0,r0,c1,c0,0

    /* invalidate caches */
    mov r0,#0x0
    mcr p15,0,r0,c7,c7,0 /* invalidate caches */
    mcr	p15,0,r0,c7,c10,4 /* drain WB */
    mcr	p15,0,r0,c8,c7,0 /* invalidate TLBs */

    /* re-enable caches & mmu */
    mrc p15,0,r0,c1,c0,0
    ldr r1,=0x00001805
    orr r0,r0,r1
    mcr p15,0,r0,c1,c0,0

    b kernel_entry
    
