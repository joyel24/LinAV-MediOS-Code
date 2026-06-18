/* 
*   kernel/lib/memchr.S
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
.global memchr
memchr:
1:	ldrb	r3, [r0], #1
	teq	r3, r1
	beq	2f
	subs	r2, r2, #1
	bpl	1b
2:	movne	r0, #0
	subeq	r0, r0, #1
	bx	lr
;; 	mov     pc,lr
