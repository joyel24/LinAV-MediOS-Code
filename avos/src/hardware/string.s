@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@ Useful string routines for converting hex etc.
@
@
@ Date:     14/02/2004
@ Author:   By DoggerMoore
@
@ stringPutHex(r0->destnation, r1=value, r2=chars)
@

.ifndef stringInc
stringInc = 1

.text
        .thumb

@ ------------------------------------------------------------------------------
@ stringPutHex r0->string, r1=value, r2=#chars
@
.globl stringPutHex
.thumb_func

stringPutHex:
        push {r0, r1, r2, r3, r4, r5, lr}
        ldr r4, =hextable
        mov r3, r2                  @ will be our shift val...
        sub r3, #1
        lsl r3, #2
stringl1:
        mov r5, r1
        lsr r5, r3
        lsl r5, #28
        lsr r5, #28
        ldrb r5, [r4, r5]
        strb r5, [r0]
        sub r3, #4                  @ do 4 bits less shift next time...
        add r0, #1
        sub r2, #1
         bne stringl1
        pop {r0, r1, r2, r3, r4, r5, pc}

hextable:   .ascii "0123456789ABCDEF"

        .ltorg
        .arm
        
.endif
