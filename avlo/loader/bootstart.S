
.text

.globl main2

main2:
    @ Setup the stack...
    mov r0, #0x8000
    mov sp, r0

    b main
    
    .space 0x4c, 0
    .asciz "CJBMVERSION2000"
    .space 0x20, 0

