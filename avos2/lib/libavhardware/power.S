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

        mov r0, #0x90
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
        pop {r1, r2, r3, r4, r5, r6, r7}
        pop {r1}
        bx r1
        
        @@ Hrm, not sure why it needs address... Doesn't seem to matter...
        
        
        ldr r7, =i2cBaseAddress
        ldrh r6, [r7, #i2cRegDR]
        mov r5, #4
        orr r6, r5
        strh r6, [r7, #i2cRegDR]

        ldrh r6, [r7, #i2cRegDR]
        mov r5, #8
        orr r6, r5
        strh r6, [r7, #i2cRegDR]

        ldrh r6, [r7, #i2cRegIN]
        lsr r6, #3
         bcc powerRE

        ldrh r6, [r7, #i2cRegIN]
        lsr r6, #4
         bcc powerRE
      
        bl i2cStart

        mov r0, #0x90               @ ==DEVICE==
        mov r6, #1
        orr r0, r6
        bl i2cOutb
        cmp r0, #0
         bne powerRE
        
        bl i2cInb
        mov r4, r0
        
        bl i2cAck

        bl i2cInb
        lsl r4, #8
        orr r4, r0
        
        bl i2cAckEnd

        bl i2cStop

        mov r0, r4
        pop {r1, r2, r3, r4, r5, r6, r7}
        pop {r1}
        bx r1
        
powerRE:
        bl i2cStop

        mov r0, #0
        sub r0, #1
        pop {r1, r2, r3, r4, r5, r6, r7}
        pop {r1}
        bx r1

pgs:    .word 0
        
        .arm
        .ltorg
