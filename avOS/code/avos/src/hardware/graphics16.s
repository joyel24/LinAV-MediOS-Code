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
.globl graphics16GetPixel
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
        push {r0, r1, r2, r4, r5, r7, lr}
        ldr r7, [r0, #GRAPHICS_BUFFER_BYTESPERLINE]
        bl graphicsGetOffset
        lsl r2, r5, #16
        lsr r5, r2, #16
        orr r5, r2                  @ r5 = | cc | cc |
                                    @ For word writes...
g16ly:  mov r1, r0
        mov r2, r3
        cmp r2, #2
         blt g16n1
g16lx:  str r5, [r0]
        add r0, #4
        sub r2, #2
        cmp r2, #2
         bge g16lx
g16n1:  cmp r2, #0
         beq g16dn                   @ r2=0, so no more needed
        strh r5, [r0]               @ Store last one...
g16dn:
        add r0, r1, r7
        sub r4, #1
         bne g16ly
        pop {r0, r1, r2, r4, r5, r7, pc}
        
@ ------------------------------------------------------------------------------
@ graphics16Sprite(r0->bufferDefDest, r1=x, r2=y, r3->bufferDefSrc)
@
.globl graphics16Sprite
.thumb_func

graphics16Sprite:
        push {r6, r7, lr}
        ldr r7, [r3, #GRAPHICS_BUFFER_BPPSHIFT]
        lsl r7, #2
        ldr r6, =graphics16SpriteRoutines
        ldr r7, [r6, r7]
        mov r6, #1
        mov lr, r6
        add lr, pc
        bx r7
        pop {r6, r7, pc}


@ ------------------------------------------------------------------------------
@ graphics16Sprite1(r0->bufferDefDest, r1=x, r2=y, r3->bufferDefSrc)
@
.globl graphics16Sprite1
.thumb_func

graphics16Sprite1:
        push {r0, r1, r2, r3, r4, r5, r6, r7, lr}
        mov r4, r0
        bl graphicsGetOffset                @ r0->TLC on dest...
        ldr r1, [r3, #GRAPHICS_BUFFER_OFFSET]
        ldr r6, [r3, #GRAPHICS_BUFFER_HEIGHT]
g16s1y: ldr r7, [r3, #GRAPHICS_BUFFER_WIDTH]
        push {r0, r1, r6}
        mov r5, #7                          @ 0-7 counter
g16s1x: 
        ldrb r2, [r1]
        lsr r2, r5
        lsl r2, #31
        lsr r2, #31
        ldr r6, [r3, #GRAPHICS_BUFFER_TRANSPARENT]
        cmp r2, r6
         beq g16s1nd        

        lsl r2, #2
        ldr r6, [r3, #GRAPHICS_BUFFER_PALLETTE16]
        ldr r2, [r6, r2]
        strh r2, [r0]
g16s1nd:
        sub r5, #1
        cmp r5, #0
         bge g16s1n
        mov r5, #7
        add r1, #1
g16s1n:
        add r0, #2
        sub r7, #1
        cmp r7, #0
         bne g16s1x
        pop {r0, r1, r6}
        ldr r5, [r4, #GRAPHICS_BUFFER_BYTESPERLINE]
        add r0, r5
        ldr r5, [r3, #GRAPHICS_BUFFER_BYTESPERLINE]
        add r1, r5
        sub r6, #1
        cmp r6, #0
         bne g16s1y
        pop {r0, r1, r2, r3, r4, r5, r6, r7}
        pop {r1}
        bx r1


@ ------------------------------------------------------------------------------
@ graphics16Sprite2(r0->bufferDefDest, r1=x, r2=y, r3->bufferDefSrc)
@
.globl graphics16Sprite2
.thumb_func

graphics16Sprite2:
        push {r0, r1, r2, r3, r4, r5, r6, r7, lr}
        mov r4, r0
        bl graphicsGetOffset                @ r0->TLC on dest...
        ldr r1, [r3, #GRAPHICS_BUFFER_OFFSET]
        ldr r6, [r3, #GRAPHICS_BUFFER_HEIGHT]
g16s2y: ldr r7, [r3, #GRAPHICS_BUFFER_WIDTH]
        push {r0, r1, r6}
        mov r5, #6                          @ 0,2,4,6 counter
g16s2x: 
        ldrb r2, [r1]
        lsr r2, r5
        lsl r2, #30
        lsr r2, #30
        ldr r6, [r3, #GRAPHICS_BUFFER_TRANSPARENT]
        cmp r2, r6
         beq g16s2nd        

        lsl r2, #2
        ldr r6, [r3, #GRAPHICS_BUFFER_PALLETTE16]
        ldr r2, [r6, r2]
        strh r2, [r0]
g16s2nd:
        sub r5, #2
        cmp r5, #0
         bge g16s2n
        mov r5, #6
        add r1, #1
g16s2n:
        add r0, #2
        sub r7, #1
        cmp r7, #0
         bne g16s2x
        pop {r0, r1, r6}
        ldr r5, [r4, #GRAPHICS_BUFFER_BYTESPERLINE]
        add r0, r5
        ldr r5, [r3, #GRAPHICS_BUFFER_BYTESPERLINE]
        add r1, r5
        sub r6, #1
        cmp r6, #0
         bne g16s2y
        pop {r0, r1, r2, r3, r4, r5, r6, r7}
        pop {r1}
        bx r1

        
@ ------------------------------------------------------------------------------
@ graphics16Sprite4(r0->bufferDefDest, r1=x, r2=y, r3->bufferDefSrc)
@
.globl graphics16Sprite4
.thumb_func

graphics16Sprite4:
        push {r0, r1, r2, r3, r4, r5, r6, r7, lr}
        mov r4, r0
        bl graphicsGetOffset                @ r0->TLC on dest...
        ldr r1, [r3, #GRAPHICS_BUFFER_OFFSET]
        ldr r6, [r3, #GRAPHICS_BUFFER_HEIGHT]
g16s4y: ldr r7, [r3, #GRAPHICS_BUFFER_WIDTH]
        push {r0, r1, r6}
        mov r5, #4                          @ 0,4 counter
g16s4x: 
        ldrb r2, [r1]
        lsr r2, r5
        lsl r2, #28
        lsr r2, #28
        ldr r6, [r3, #GRAPHICS_BUFFER_TRANSPARENT]
        cmp r2, r6
         beq g16s4nd        

        lsl r2, #2
        ldr r6, [r3, #GRAPHICS_BUFFER_PALLETTE16]
        ldr r2, [r6, r2]
        strh r2, [r0]
g16s4nd:
        sub r5, #4
        cmp r5, #0
         bge g16s4n
        mov r5, #4
        add r1, #1
g16s4n:
        add r0, #2
        sub r7, #1
        cmp r7, #0
         bne g16s4x
        pop {r0, r1, r6}
        ldr r5, [r4, #GRAPHICS_BUFFER_BYTESPERLINE]
        add r0, r5
        ldr r5, [r3, #GRAPHICS_BUFFER_BYTESPERLINE]
        add r1, r5
        sub r6, #1
        cmp r6, #0
         bne g16s4y
        pop {r0, r1, r2, r3, r4, r5, r6, r7}
        pop {r1}
        bx r1
        
        
@ ------------------------------------------------------------------------------
@ graphics32Sprite8(r0->bufferDefDest, r1=x, r2=y, r3->bufferDefSrc)
@
.globl graphics16Sprite8
.thumb_func

graphics16Sprite8:
        push {r0, r1, r2, r3, r4, r5, r6, r7, lr}
        mov r4, r0
        bl graphicsGetOffset                @ r0->TLC on dest...
        ldr r1, [r3, #GRAPHICS_BUFFER_OFFSET]
        ldr r6, [r3, #GRAPHICS_BUFFER_HEIGHT]
g16s8y: ldr r7, [r3, #GRAPHICS_BUFFER_WIDTH]
        push {r0, r1, r6}
g16s8x: mov r2, #0
        ldrb r2, [r1]
        ldr r6, [r3, #GRAPHICS_BUFFER_TRANSPARENT]
        cmp r2, r6
         beq g16s8nd        

        lsl r2, #2
        ldr r6, [r3, #GRAPHICS_BUFFER_PALLETTE16]
        ldr r2, [r6, r2]
        strh r2, [r0]
g16s8nd:
        add r1, #1
        add r0, #2
        sub r7, #1
        cmp r7, #0
         bne g16s8x
        pop {r0, r1, r6}
        ldr r5, [r4, #GRAPHICS_BUFFER_BYTESPERLINE]
        add r0, r5
        ldr r5, [r3, #GRAPHICS_BUFFER_BYTESPERLINE]
        add r1, r5
        sub r6, #1
        cmp r6, #0
         bne g16s8y
        pop {r0, r1, r2, r3, r4, r5, r6, r7}
        pop {r1}
        bx r1

        
@ ------------------------------------------------------------------------------
@ graphics16Sprite16(r0->bufferDefDest, r1=x, r2=y, r3->bufferDefSrc)
@
.globl graphics16Sprite16
.thumb_func

graphics16Sprite16:
        push {r0, r1, r2, r3, r4, r5, r6, r7, lr}
        mov r4, r0
        bl graphicsGetOffset                @ r0->TLC on dest...
        ldr r1, [r3, #GRAPHICS_BUFFER_OFFSET]
        ldr r6, [r3, #GRAPHICS_BUFFER_HEIGHT]
        ldr r5, [r4, #GRAPHICS_BUFFER_BYTESPERLINE]
        ldr r4, [r3, #GRAPHICS_BUFFER_TRANSPARENT]

        ldr r7, [r3, #GRAPHICS_BUFFER_WIDTH]
        lsl r7, #1
        ldr r2, [r3, #GRAPHICS_BUFFER_BYTESPERLINE]
g16s16y:
        push {r0, r1, r2, r6, r7}
        mov r6, #0
g16s16x:
        ldrh r2, [r1, r6]
        cmp r2, r4
         beq g16s16nd
        strh r2, [r0, r6]
g16s16nd:
        add r6, #2
        cmp r6, r7
         bne g16s16x
        pop {r0, r1, r2, r6, r7}
        add r0, r5
        add r1, r2
        sub r6, #1
         bne g16s16y
        pop {r0, r1, r2, r3, r4, r5, r6, r7}
        pop {r1}
        bx r1
        
        
@ ------------------------------------------------------------------------------
@ graphics16Sprite32(r0->bufferDefDest, r1=x, r2=y, r3->bufferDefSrc)
@
.globl graphics16Sprite32
.thumb_func

graphics16Sprite32:
        @ DIFFICULT - REQUIRES LOTS OF WORK...
        mov pc, lr

        .align 4
graphics16SpriteRoutines:   .word graphics16Sprite1, graphics16Sprite2
                            .word graphics16Sprite4, graphics16Sprite8
                            .word graphics16Sprite16,graphics16Sprite32
        
        
        .arm
        .ltorg

.endif
