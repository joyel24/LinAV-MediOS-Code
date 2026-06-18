.include "../include/target/arch/asm_def.h"

.text

.globl entry_code

entry_code:
    @ Setup the stack...
    mov r0, #0x8000
    mov sp, r0

    b move_code
    
    .space 0x4c, 0
    .asciz "CJBMVERSION2000"
    .space 0x20, 0

move_code:
    ldr r0, =ramStart
    ldr r1, __start_kernel_ptr
    ldr r2, __end_kernel_ptr
    sub r2, r2, r1
mv_code_loop:
    ldr     r3, [r0],#4
    str     r3, [r1],#4
    subs    r2, r2, #4
    bne     mv_code_loop
    ldr    r3, __code_start_ptr
    mov    lr,pc
    bx     r3
end_loop:
    b end_loop
    

__bss_start_ptr:       .word _bss_start
__bss_end_ptr:         .word _bss_end
__start_kernel_ptr:    .word _start_kernel
__end_kernel_ptr:      .word _end_kernel
__code_start_ptr:      .word start_avlo
