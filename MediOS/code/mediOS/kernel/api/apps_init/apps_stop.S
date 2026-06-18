/* 
*   kernel/lib/apps_init/apps_stop.S
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*/


.text



.global exit
exit:
    #ldr     r2, =gfx_closeGraphics
    #ldr     r2,[r2]
    #mov     lr, pc
    #mov     pc, r2
    ldr     r0, =.SavSP
    ldr     sp, [r0]
    ldmia   sp, {r0 - r12}			@ read r0 - r12
    add     sp, sp, #72
    mov     r0, #0
    ldr     pc, [sp],#4
