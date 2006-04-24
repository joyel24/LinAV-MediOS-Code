/*
*   kernel/target/arch/boot.S
*   Av3XX boot
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/
.section ".text.init.arch"
.global entry_code
entry_code:
    /* av3 init */
    mov SP,#0x8000
    ldr r0,=0x30920
    ldrh r0,[r0]
    cmp r0,#8
    beq kernel_entry
    bl  code_cpy
    mov r0,#0x30
    mov r1,#0x92
    add r1,r1,r0,lsl #8
    mov r0,r1, lsl #4
    mov r1,#8
    strh r1,[r0]
    mov r1,#0x20
    strh r1,[r0,#0x22]
    mov r0,#9
    mov r0, r0, lsl #20
    mov pc, r0
code_cpy:
    mov r2, pc
    mov r0, lr    
    sub r2,r2,r0
    sub SP,SP,r2
    mov r1, SP
code_cpy_loop:
    ldr     r3, [r0],#4
    str     r3, [r1],#4
    subs    r2, r2, #4
    bne     code_cpy_loop  
    mov pc,SP
    