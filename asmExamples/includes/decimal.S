@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@ Decimal
@
@
@ Date:     04/01/2004
@ Author:   By DoggerMoore
@
@ dec5T(r0->destnation, r1=value)
@

@    .include "../includes/common.S"
@    .include "../includes/math.S"
    
.ifndef decInc
decInc = 1

.text

        .thumb

@ dec5T  r0->dest, r1=value
@
.globl dec5T
.thumb_func
dec5T:  push {lr}
        push {r1, r2, r3, r4, r5}
        mov r3, #5
        mov r4, r0
        sub r4, #1
        mov r2, #10                     @ base = 10
decL1:  mov r0, #0
        callThumb r5, mathDivLU         @ divide
        add r0, #'0'                    @ convert to ascii
        strb r0, [r4, r3]
        sub r3, #1
         bne decL1
        pop {r1, r2, r3, r4, r5}
        pop {r0}
        bx r0

        .ltorg
        .arm
        
.endif
