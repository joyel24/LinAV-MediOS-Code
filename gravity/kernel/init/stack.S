        .section ".text"
        .type    get_sp,#function
.global get_sp
get_sp:
    mov r0, sp
    mov pc, lr   
    