@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@ ecr
@
@ Date:     21/02/2004
@ Author:   By DoggerMoore
@
@ u32 ecbSwapBytes(u32 value)
@ u32 ecbSwapBits(u32 value)
@ u32 ecbSwapBytesBuffer(u32 offset, u32 count)
@ u32 ecbSwapBitsBuffer(u32 offset, u32 count)
@

.ifndef ecrInc
ecrInc = 1

.text

@ ------------------------------------------------------------------------------
@ ecrSwapBytes()
@   fedcba98 76543210 ->    76543210 fedcba98
.globl ecrSwapBytes
.thumb_func

ecrSwapBytes:
        push {r1}
        ldr r1, =0x30900
        strh r0, [r1]
        ldrh r0, [r1, #2]
        pop {r1}
        bx lr

        
@ ------------------------------------------------------------------------------
@ ecrSwapBytesBuffer(r0->buffer, r1->bufferOut, r2=count)
@   fedcba98 76543210 ->    76543210 fedcba98
.globl ecrSwapBytesBuffer
.thumb_func

ecrSwapBytesBuffer:
        push {r0, r1, r2, r3, r4}
        ldr r4, =0x30900
        sub r2, #2
ecrl1:  ldrh r3, [r0, r2]
        strh r3, [r4, r2]
        ldrh r3, [r4, #2]
        strh r3, [r1]
        sub r2, #2
         bcc ecrl1
        pop {r0, r1, r2, r3, r4}
        bx lr

        
@ ------------------------------------------------------------------------------
@ ecrSwapBits()
@   fedcba98 76543210 ->    01234567 89abcdef
.globl ecrSwapBits
.thumb_func

ecrSwapBits:
        push {r1}
        ldr r1, =0x30900
        strh r0, [r1]
        ldrh r0, [r1, #4]
        pop {r1}
        bx lr


@ ------------------------------------------------------------------------------
@ ecrSwapBitsBuffer(r0->buffer, r1->bufferOut, r2=count)
@   fedcba98 76543210 ->    01234567 89abcdef
.globl ecrSwapBitsBuffer
.thumb_func

ecrSwapBitsBuffer:
        push {r0, r1, r2, r3, r4}
        ldr r4, =0x30900
        sub r2, #2
ecrl2:  ldrh r3, [r0, r2]
        strh r3, [r4]
        ldrh r3, [r4, #4]
        strh r3, [r1, r2]
        sub r2, #2
         bcc ecrl2
        pop {r0, r1, r2, r3, r4}
        bx lr
        
        .arm
        .ltorg

.endif
