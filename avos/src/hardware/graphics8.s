@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@ Graphics8 - General graphics routines (8 bit)
@
@ Date:     10/02/2004
@ Author:   By DoggerMoore
@
@

.ifndef graphics8Inc
graphics8Inc = 1

.text
        
@ ------------------------------------------------------------------------------
@ graphics8SetPixel(r0->bufferDef, r1=x, r2=y, r3=c)
@
.globl graphics8SetPixel
.thumb_func

graphics8SetPixel:
        push {r0, lr}
        bl graphicsGetOffset
        strb r3, [r0]
        pop {r0, pc}


@ ------------------------------------------------------------------------------
@ graphics8GetPixel(r0->bufferDef, r1=x, r2=y)
@       Returns the pixels color in r0
@
.globl graphics8GetPixel
.thumb_func

graphics8GetPixel:
        push {r1, lr}
        bl graphicsGetOffset
        mov r1, r0
        mov r0, #0
        ldrb r0, [r1]
        pop {r1, pc}
        

@ ------------------------------------------------------------------------------
@ graphics8BoxfR(r0->bufferDef, r1=x, r2=y, r3=width, r4=height, r5=c)
@
.globl graphics8BoxfR
.thumb_func

graphics8BoxfR:
        push {r0, r1, r2, r4, r7, lr}
        ldr r7, [r0, #GRAPHICS_BUFFER_BYTESPERLINE]
        bl graphicsGetOffset
g8ly:   mov r1, r0
        mov r2, r3
g8lx:   strb r5, [r0]
        add r0, #1
        sub r2, #1
        cmp r2, #0
         bne g8lx
        mov r0, r1
        add r0, r7
        sub r4, #1
        cmp r4, #0
         bne g8ly
        pop {r0, r1, r2, r4, r7, pc}
        
        
@ ------------------------------------------------------------------------------
@ graphics8Sprite(r0->bufferDefDest, r1=x, r2=y, r3->bufferDefSrc)
@
.globl graphics8Sprite
.thumb_func

graphics8Sprite:
        mov pc, lr
        
        
        .arm
        .ltorg

.endif
