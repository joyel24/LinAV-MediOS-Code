@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@ Graphics2 - General graphics routines (2 bit)
@
@ Date:     10/02/2004
@ Author:   By DoggerMoore
@
@

.ifndef graphics2Inc
graphics2Inc = 1

.text
        
@ ------------------------------------------------------------------------------
@ graphics2SetPixel(r0->bufferDef, r1=x, r2=y, r3=c)
@
.globl graphics2SetPixel
.thumb_func

graphics2SetPixel:
        push {r1, r2, r3, lr}
        pop {r1, r2, r3, pc}


@ ------------------------------------------------------------------------------
@ graphics2GetPixel(r0->bufferDef, r1=x, r2=y)
@       Returns the pixels color in r0
@
.globl graphics2SetPixel
.thumb_func

graphics2GetPixel:
        push {r1, r2, r3, lr}
        pop {r1, r2, r3, pc}

        
@ ------------------------------------------------------------------------------
@ graphics2BoxfR(r0->bufferDef, r1=x, r2=y, r3=width, r4=height, r5=c)
@
.globl graphics2BoxfR
.thumb_func

graphics2BoxfR:
        mov pc, lr
        
        
@ ------------------------------------------------------------------------------
@ graphics2Sprite(r0->bufferDefDest, r1=x, r2=y, r3->bufferDefSrc)
@
.globl graphics2Sprite
.thumb_func

graphics2Sprite:
        mov pc, lr
        
        
        .arm
        .ltorg

.endif
