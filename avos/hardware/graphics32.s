@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@ Graphics32 - General graphics routines (32 bit)
@
@ Date:     10/02/2004
@ Author:   By DoggerMoore
@
@

.ifndef graphics32Inc
graphics32Inc = 1

.text
        
@ ------------------------------------------------------------------------------
@ graphics32SetPixel(r0->bufferDef, r1=x, r2=y, r3=c)
@
.globl graphics32SetPixel
.thumb_func

graphics32SetPixel:
        push {r0, lr}
        bl graphicsGetOffset
        str r3, [r0]
        pop {r0, pc}


@ ------------------------------------------------------------------------------
@ graphics32GetPixel(r0->bufferDef, r1=x, r2=y)
@       Returns the pixels color in r0
@
.globl graphics32GetPixel
.thumb_func

graphics32GetPixel:
        push {lr}
        bl graphicsGetOffset
        ldr r0, [r0]
        pop {pc}

        
@ ------------------------------------------------------------------------------
@ graphics32BoxfR(r0->bufferDef, r1=x, r2=y, r3=width, r4=height, r5=c)
@
.globl graphics32BoxfR
.thumb_func

graphics32BoxfR:
        push {r0, r1, r2, r4, r7, lr}
        ldr r7, [r0, #GRAPHICS_BUFFER_BYTESPERLINE]
        bl graphicsGetOffset
g32ly:  mov r1, r0
        mov r2, r3
g32lx:  str r5, [r0]
        add r0, #4
        sub r2, #1
        cmp r2, #0
         bne g32lx
        mov r0, r1
        add r0, r7
        sub r4, #1
        cmp r4, #0
         bne g32ly
        pop {r0, r1, r2, r4, r7, pc}
        
@ ------------------------------------------------------------------------------
@ graphics32Boxf(r0->bufferDef, r1=x, r2=y, r3=width, sp1=height, sp2=c)
@
.globl graphics32Boxf
.thumb_func
graphics32Boxf:
        push {r4, r5, lr}
        ldr r4, [sp, #12]
        ldr r5, [sp, #16]
        bl graphics32Boxf
        pop {r4, r5, pc}

        .arm
        .ltorg

.endif
