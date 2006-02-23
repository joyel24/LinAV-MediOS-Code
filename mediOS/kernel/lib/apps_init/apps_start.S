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
    ldr     r2, =ini_api
    mov     lr, pc
    mov     pc, r2
    ldr     r2, =app_main
    mov     lr, pc
    mov     pc, r2
    bl      exit

.global .SavSP
.SavSP: .word 0
