@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@ Graphics16 - General graphics routines (16 bit)
@
@ Date:     10/02/2004
@ Author:   By DoggerMoore
@
@

.ifndef graphics16Inc
graphics16Inc = 1

.text
        
@ ------------------------------------------------------------------------------
@ graphics16SetPixel(r0->bufferDef, r1=x, r2=y, r3=c)
@
.globl graphics16SetPixel
.thumb_func

graphics16SetPixel:
        push {r0, lr}
        bl graphicsGetOffset
        strh r3, [r0]
        pop {r0, pc}


@ ------------------------------------------------------------------------------
@ graphics16GetPixel(r0->bufferDef, r1=x, r2=y)
@       Returns the pixels color in r0
@
.globl graphics16SetPixel
.thumb_func

graphics16GetPixel:
        push {r1, lr}
        bl graphicsGetOffset
        mov r1, r0
        mov r0, #0
        ldrh r0, [r1]
        pop {r1, pc}


@ ------------------------------------------------------------------------------
@ graphics16BoxfR(r0->bufferDef, r1=x, r2=y, r3=width, r4=height, r5=c)
@
.globl graphics16BoxfR
.thumb_func

graphics16BoxfR:
        push {r0, r1, r2, r4, r7, lr}
        ldr r7, [r0, #GRAPHICS_BUFFER_BYTESPERLINE]
        bl graphicsGetOffset
g16ly:  mov r1, r0
        mov r2, r3
g16lx:  strh r5, [r0]
        add r0, #2
        sub r2, #1
        cmp r2, #0
         bne g16lx
        mov r0, r1
        add r0, r7
        sub r4, #1
        cmp r4, #0
         bne g16ly
        pop {r0, r1, r2, r4, r7, pc}
        
@ ------------------------------------------------------------------------------
@ graphics16Sprite(r0->bufferDefDest, r1=x, r2=y, r3->bufferDefSrc)
@
.globl graphics16Sprite
.thumb_func

graphics16Sprite:
        mov pc, lr
        
        
        .arm
        .ltorg

.endif
