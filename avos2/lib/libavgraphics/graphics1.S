@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@ Graphics1 - General graphics routines (1 bit)
@
@ Date:     10/02/2004
@ Author:   By DoggerMoore
@
@

.ifndef graphics1Inc
graphics1Inc = 1

.text
        
@ ------------------------------------------------------------------------------
@ graphics1SetPixel(r0->bufferDef, r1=x, r2=y, r3=c)
@
.globl graphics1SetPixel
.thumb_func

graphics1SetPixel:
        push {r1, r2, r3, lr}
        pop {r1, r2, r3, pc}


@ ------------------------------------------------------------------------------
@ graphics1GetPixel(r0->bufferDef, r1=x, r2=y)
@       Returns the pixels color in r0
@
.globl graphics1GetPixel
.thumb_func

graphics1GetPixel:
        push {r1, r2, r3, lr}
        pop {r1, r2, r3, pc}

        
@ ------------------------------------------------------------------------------
@ graphics1BoxfR(r0->bufferDef, r1=x, r2=y, r3=width, r4=height, r5=c)
@
.globl graphics1BoxfR
.thumb_func

graphics1BoxfR:
        mov pc, lr

        
@ ------------------------------------------------------------------------------
@ graphics1Sprite(r0->bufferDefDest, r1=x, r2=y, r3->bufferDefSrc)
@
.globl graphics1Sprite
.thumb_func

graphics1Sprite:
        mov pc, lr
        
        
        .arm
        .ltorg

.endif
