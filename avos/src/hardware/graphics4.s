@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@ Graphics4 - General graphics routines (4 bit)
@
@ Date:     10/02/2004
@ Author:   By DoggerMoore
@
@

.ifndef graphics4Inc
graphics4Inc = 1

.text
        
@ ------------------------------------------------------------------------------
@ graphics4SetPixel(r0->bufferDef, r1=x, r2=y, r3=c)
@
.globl graphics4SetPixel
.thumb_func

graphics4SetPixel:
        push {r1, r2, r3, lr}
        pop {r1, r2, r3, pc}


@ ------------------------------------------------------------------------------
@ graphics4GetPixel(r0->bufferDef, r1=x, r2=y)
@       Returns the pixels color in r0
@
.globl graphics4SetPixel
.thumb_func

graphics4GetPixel:
        push {r1, r2, r3, lr}
        pop {r1, r2, r3, pc}

        
@ ------------------------------------------------------------------------------
@ graphics4BoxfR(r0->bufferDef, r1=x, r2=y, r3=width, r4=height, r5=c)
@
.globl graphics4BoxfR
.thumb_func

graphics4BoxfR:
        mov pc, lr


@ ------------------------------------------------------------------------------
@ graphics4Sprite(r0->bufferDefDest, r1=x, r2=y, r3->bufferDefSrc)
@
.globl graphics4Sprite
.thumb_func

graphics4Sprite:
        mov pc, lr
        
        
        .arm
        .ltorg

.endif
