/* 
*   kernel/0boot.S
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/
    .section ".text.init"
.global entry_code
entry_code:
        b chg_stack
        .space 0x10000
@.global stack_start
chg_stack:
    ldr r0, =chg_stack
@    mov r0, #0x8000
    sub r0, r0, #4
    mov sp, r0
    ldr    lr, __kernel_start_ptr
    bx     lr

__kernel_start_ptr:  .word kernel_start

      