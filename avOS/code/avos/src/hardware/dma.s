@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@ dma
@
@ Date:     01/03/2004
@ Author:   By DoggerMoore
@
@ dmaSetSource(u32 address)
@ dmaSetDest(u32 address)
@ dmaSetSize(u32 size)
@ dmaDevSelect(u32 src, u32 dest)
@ dmaStart(u32 endianness)
@ dmaIsRunning()


.macro switchThumb
        .arm
        add ip, pc, #1
        bx ip
        .thumb
.endm
.ifndef dmaInc
dmaInc = 1

.text

@ ------------------------------------------------------------------------------
@ dmaSetSource(u32 address)
@
.globl dmaSetSourceA
dmaSetSourceA:
        switchThumb
.globl damSetSource
.thumb_func

dmaSetSource:
        push {r1}
        ldr r1, =0x30a38
        strh r0, [r1, #2]
        lsr r0, #16
        strh r0, [r1]
        pop {r1}
        bx lr

@ ------------------------------------------------------------------------------
@ dmaSetDest(u32 address)
@
.globl dmaSetDestA
dmaSetDestA:
        switchThumb
.globl damSetDest
.thumb_func

dmaSetDest:
        push {r1}
        ldr r1, =0x30a3c
        strh r0, [r1, #2]
        lsr r0, #16
        strh r0, [r1]
        pop {r1}
        bx lr
        
@ ------------------------------------------------------------------------------
@ dmaSetSize(u32 size)
@
.globl dmaSetSizeA
dmaSetSizeA:
        switchThumb
.globl damSetSize
.thumb_func

dmaSetSize:
        push {r1}
        ldr r1, =0x30a40
        strh r0, [r1]
        pop {r1}
        bx lr

@ ------------------------------------------------------------------------------
@ dmaDevSelect(u32 devSrc, u32 devDest)
@
.globl dmaDevSelectA
dmaDevSelectA:
        switchThumb
.globl damDevSelect
.thumb_func

dmaDevSelect:
        push {r2}
        ldr r2, =0x30a42
        lsl r0, #4
        orr r0, r1
        strh r0, [r2]
        pop {r2}
        bx lr

@ ------------------------------------------------------------------------------
@ dmaStart(u32 endianness)
@
.globl dmaStartA
dmaStartA:
        switchThumb
.globl damStart
.thumb_func

dmaStart:
        push {r1, r2}
        ldr r2, =0x30a44
        mov r1, #1
        orr r0, r1
        strh r0, [r2]
        pop {r1, r2}
        bx lr
        
@ ------------------------------------------------------------------------------
@ dmaIsRunning()
@
.globl dmaIsRunningA
dmaIsRunningA:
        switchThumb
.globl damIsRunning
.thumb_func

dmaIsRunning:
        push {r2}
        ldr r2, =0x30a44
        ldrh r0, [r2]
        lsl r0, #31
        lsr r0, #31
        pop {r2}
        bx lr

        .arm
        .ltorg

.endif
