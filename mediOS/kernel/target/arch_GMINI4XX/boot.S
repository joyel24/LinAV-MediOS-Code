/*
*   kernel/target/arch/boot.S
*   Gmini400 boot
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/
.section ".text.init.arch"
.global entry_code
entry_code:
    /* g4 init */

    b kernel_entry
    