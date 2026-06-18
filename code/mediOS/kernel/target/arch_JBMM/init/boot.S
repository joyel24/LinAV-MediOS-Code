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
    /* no specific init for av1xx*/
    b kernel_entry
    