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
        bl graphics32BoxfR
        pop {r4, r5, pc}

@ ------------------------------------------------------------------------------
@ graphics32Sprite(r0->bufferDefDest, r1=x, r2=y, r3->bufferDefSrc)
@
.globl graphics32Sprite
.thumb_func

graphics32Sprite:
        push {r6, r7, lr}
        ldr r7, [r3, #GRAPHICS_BUFFER_BPPSHIFT]
        lsl r7, #2
        ldr r6, =graphics32SpriteRoutines
        ldr r7, [r6, r7]
        mov lr, pc
        mov pc, r7
        pop {r6, r7, pc}

        
@ ------------------------------------------------------------------------------
@ graphics32Sprite1(r0->bufferDefDest, r1=x, r2=y, r3->bufferDefSrc)
@
.globl graphics32Sprite1
.thumb_func

graphics32Sprite1:
        push {r0, r1, r2, r3, r4, r5, r6, r7, lr}
        mov r4, r0
        bl graphicsGetOffset                @ r0->TLC on dest...
        ldr r1, [r3, #GRAPHICS_BUFFER_OFFSET]
        ldr r6, [r3, #GRAPHICS_BUFFER_HEIGHT]
g32s1y: ldr r7, [r3, #GRAPHICS_BUFFER_WIDTH]
        push {r0, r1, r6}
        mov r5, #7                          @ 0-7 counter
g32s1x: 
        ldrb r2, [r1]
        lsr r2, r5
        lsl r2, #31
        lsr r2, #31
        ldr r6, [r3, #GRAPHICS_BUFFER_TRANSPARENT]
        cmp r2, r6
         beq g32s1nd        

        lsl r2, #2
        ldr r6, [r3, #GRAPHICS_BUFFER_PALLETTE]
        ldr r2, [r6, r2]
        str r2, [r0]
g32s1nd:
        sub r5, #1
        cmp r5, #0
         bge g32s1n
        mov r5, #7
        add r1, #1
g32s1n:
        add r0, #4
        sub r7, #1
        cmp r7, #0
         bne g32s1x
        pop {r0, r1, r6}
        ldr r5, [r4, #GRAPHICS_BUFFER_BYTESPERLINE]
        add r0, r5
        ldr r5, [r3, #GRAPHICS_BUFFER_BYTESPERLINE]
        add r1, r5
        sub r6, #1
        cmp r6, #0
         bne g32s1y
        pop {r0, r1, r2, r3, r4, r5, r6, r7, pc}


@ ------------------------------------------------------------------------------
@ graphics32Sprite2(r0->bufferDefDest, r1=x, r2=y, r3->bufferDefSrc)
@
.globl graphics32Sprite2
.thumb_func

graphics32Sprite2:
        mov pc, lr

        
@ ------------------------------------------------------------------------------
@ graphics32Sprite4(r0->bufferDefDest, r1=x, r2=y, r3->bufferDefSrc)
@
.globl graphics32Sprite4
.thumb_func

graphics32Sprite4:
        mov pc, lr
        
        
@ ------------------------------------------------------------------------------
@ graphics32Sprite8(r0->bufferDefDest, r1=x, r2=y, r3->bufferDefSrc)
@
.globl graphics32Sprite8
.thumb_func

graphics32Sprite8:
        mov pc, lr
        
        
@ ------------------------------------------------------------------------------
@ graphics32Sprite16(r0->bufferDefDest, r1=x, r2=y, r3->bufferDefSrc)
@
.globl graphics32Sprite16
.thumb_func

graphics32Sprite16:
        mov pc, lr
        
        
@ ------------------------------------------------------------------------------
@ graphics32Sprite32(r0->bufferDefDest, r1=x, r2=y, r3->bufferDefSrc)
@
.globl graphics32Sprite32
.thumb_func

graphics32Sprite32:
        mov pc, lr
        
        
graphics32SpriteRoutines:   .word graphics32Sprite1, graphics32Sprite2
                            .word graphics32Sprite4, graphics32Sprite8
                            .word graphics32Sprite16,graphics32Sprite32
        
        
        .arm
        .ltorg

.endif
