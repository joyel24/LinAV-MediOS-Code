/* 
*   kernel/core/delay.S
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*   Code from uClinux
*   Copyright (C) 1995, 1996 Russell King
*/

    .text


/*
 * 0 <= r0 <= 2000
 */
.global udelay
udelay:
    mov    r2, #0x6800
    orr    r2, r2, #0x00db
    mul    r1, r0, r2
    ldr    r2, loops_val
    mov    r1, r1, lsr #11
    mov    r2, r2, lsr #11
    mul    r0, r1, r2
    movs   r0, r0, lsr #6
	bxeq	lr
;; 	moveq pc, lr
udelay_l:
    subs   r0, r0, #1
    bhi    udelay_l
	bx	lr
;; 	mov pc, lr
    
loops_val: .word 12384
