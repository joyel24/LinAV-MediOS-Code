@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@ Math
@
@ Date:     25/01/2004
@ Author:   By DoggerMoore
@
@ mathDivLU (x, y, z)   -> r0=(x||y) % z, r1 = (x||y) / z
@

.ifndef mathInc
mathInc = 1

.text
        .thumb

@ u32 mathDivLU(r0= x, r1= y, r2= z) 
@       returns r0 = (x||y) % z
@               r1 = (x||y) / z
@
.globl mathDivLU
.thumb_func
mathDivLU:
        push {r2, r3, r4, r5, lr}
        mov r3, #1
mathL1: lsr r4, r0, #31
        lsl r0, #1
        lsr r5, r1, #31
        orr r0, r5
        lsl r1, #1
        orr r4, r0
        cmp r4, r2
         blt mathL2
        sub r0, r2
        add r1, #1
mathL2: add r3, #1
        cmp r3, #33
         bne mathL1
        pop {r2, r3, r4, r5, pc}

        .arm
        .ltorg
.endif
