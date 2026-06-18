/* 
*   kernel/lib/strrchr.S
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
		.align	5
.global strrchr
strrchr:
		mov	r3, #0
1:		ldrb	r2, [r0], #1
		teq	r2, r1
		subeq	r3, r0, #1
		teq	r2, #0
		bne	1b
		mov	r0, r3
		bx	lr
;;		mov pc, lr
