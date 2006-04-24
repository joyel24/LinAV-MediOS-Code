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
    /* g4 init */

    /* set UART0 to 115200 bauds */
    ldr r5,=0x30302
    mov r1,#0x36
    strh r1,[r5]

    /*
    * cache init
    */
    ldr r5,=0x30e00
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
    ldr r1,=0x0101
    strh r1,[r5]

    b kernel_entry
    