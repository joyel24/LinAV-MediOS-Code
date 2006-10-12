/*
*   kernel/target/arch/boot.S
*   Av1XX boot
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/
.section ".text.init.arch"
.global entry_code
entry_code:
    /* av4xx init : we should re-enable cache */   
    
    /* call main init*/
    b kernel_entry
    