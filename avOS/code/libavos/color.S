@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@ color
@
@ Date:     15/01/2004
@ Author:   By DoggerMoore
@
@ colorRGB2YCbCrT r0=r,r1=g,r2=b -> r0=Y,r1=Cr,r2=Cb
@ colorPackYCbCrT r0=Y, r1=Cr, r2=Cb -> r0=packed
@

.ifndef colorInc
colorInc = 1

.text
        .thumb

@ colorPackYCbCrT r0=Y, r1=Cr, r2=Cb -> r0=packed
@
.globl colorPackYCbCrT
.thumb_func
colorPackYCbCrT:
        lsl r0, #8
        orr r0, r2
        lsl r1, #16
        orr r0, r1
        bx lr
        
@ colorRGB2YCbCrT r0=r,r1=g,r2=b -> r0=Y,r1=Cr,r2=Cb
@       y = 0.2990r + 0.5870g + 0.1140b
@       cb=-0.1687r - 0.3313g + 0.5000b + 128
@       cr= 0.5000r - 0.4187g - 0.0813b + 128
@
.globl colorRGB2YCbCrT
.thumb_func
colorRGB2YCbCrT:
        push {r3, r4, r5, r6}
        
        ldr r3, =19595
        mul r3, r0
        ldr r4, =38470
        mul r4, r1
        add r3, r4
        ldr r4, =7471
        mul r4, r2
        add r3, r4                                         @ r3 = Y
        
        mov r4, r2
        lsl r4, #15
        ldr r5, =21712
        mul r5, r1
        sub r4, r5
        ldr r5, =11056
        mul r5, r0
        sub r4, r5
        ldr r5, =0x00800000
        add r4, r5                                         @ r4 = Cb
        
        mov r5, r0
        lsl r5, #15
        ldr r6, =27440
        mul r6, r1
        sub r5, r6
        ldr r6, =5328
        mul r6, r2
        sub r5, r6
        ldr r6, =0x00800000
        add r5, r6

        mov r0, r3
        mov r1, r5
        mov r2, r4
        asr r0, #16
        asr r1, #16
        asr r2, #16

        cmp r0, #0
         bge colorO1
        mov r0, #0
colorO1:cmp r0, #255
         ble colorO2
        mov r0, #255
colorO2:cmp r1, #0
         bge colorO3
        mov r1, #0
colorO3:cmp r1, #255
         ble colorO4
        mov r1, #255
colorO4:cmp r2, #0
         bge colorO5
        mov r2, #0
colorO5:cmp r2, #255
         ble colorO6
        mov r2, #255
colorO6:pop {r3, r4, r5, r6}
        bx lr

        .ltorg
    
        .arm
.endif
