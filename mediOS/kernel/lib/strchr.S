/* 
*   kernel/lib/strchr.S
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*   Code from uClinux
*   Copyright (C) 1995, 1996 Russell King
*
*  ASM optimised string functions
*/
    
              .text
              .align       5
.global strchr
strchr:
1:            
    ldrb      r2, [r0], #1
    teq       r2, r1
    teqne     r2, #0
    bne       1b
    teq       r2, #0
    moveq     r0, #0
    subne     r0, r0, #1
	bx	lr
;; 	mov pc, lr

