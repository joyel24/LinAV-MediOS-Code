@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@ Power
@
@ Date:     20/02/2004
@ Author:   By DoggerMoore
@
.macro switchThumb
        .arm
        add ip, pc, #1
        bx ip
        .thumb
.endm

.include "include/i2c.inc"
        
.text
        .thumb

@-------------------------------------------------------------------------------
@ powerIsDCConnected()
@
.globl powerIsDCConnectedA
powerIsDCConnectedA:
        switchThumb
.globl powerIsDCConnected
.thumb_func

powerIsDCConnected:
        push {r1}
        ldr r1, =0x30a24
        ldrh r0, [r1]
        lsr r0, #5
        mov r1, #1
        and r0, r1
        pop {r1}
        bx lr    

        
@-------------------------------------------------------------------------------
@ powerGetStatus()
@
.globl powerGetStatusA
powerGetStatusA:
        switchThumb
.globl powerGetStatus
.thumb_func

powerGetStatus:
        push {r1, r2, r3, r4, r5, r6, r7, lr}

        mov r0, #0x90            @ 10010000
        mov r1, #0x00            @ no idea if this is anything
        ldr r2, =pgs
        mov r3, #2
        bl i2cRead
        
        mov r0, #0
        mov r1, #0
        ldrb r0, [r2]
        lsl r0, #8
        ldrb r1, [r2, #1]
        orr r0, r1
        lsr r0, #4
        pop {r1, r2, r3, r4, r5, r6, r7}
        pop {r1}
        bx r1

pgs:    .word 0
        
        .arm
        .ltorg
