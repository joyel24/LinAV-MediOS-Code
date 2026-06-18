/* 
*   kernel/lib/apps_init/apps_start.S
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*/


.text

.global _start
_start:
    str     lr, [sp,#-4]!    
    sub     sp, sp, #72
    stmia   sp, {r0 - r12}			@ save r0 - r12
    ldr     lr, =.SavSP
    str     sp, [lr]
    mov     r10, r0
    mov     r11, r1
    ldr     r2, =ini_api
    mov     lr, pc
    mov     pc, r2
    mov     r0, r10
    mov     r1, r11
    ldr     r2, =app_main
    mov     lr, pc
    mov     pc, r2
    bl      exit

.global .SavSP
.SavSP: .word 0
