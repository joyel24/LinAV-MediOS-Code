@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@ Graphics - General graphics routines
@
@ Date:     10/02/2004
@ Author:   By DoggerMoore
@
@

.ifndef graphicsInc
graphicsInc = 1

.text
        GRAPHICS_BUFFER_OFFSET      =   0x0000      @ Offset of buffer
        GRAPHICS_BUFFER_BYTESPERLINE=   0x0004      @ Bytes per scanline
        GRAPHICS_BUFFER_WIDTH       =   0x0008      @ Width in pixels
        GRAPHICS_BUFFER_HEIGHT      =   0x000c      @ Height in pixels
        GRAPHICS_BUFFER_BITSPERPIXEL=   0x0010      @ Bits per pixel
        GRAPHICS_BUFFER_SIZE        =   0x0014
        
        GRAPHICS_ROUTINE_SETPIXEL   =   0x0000
        GRAPHICS_ROUTINE_GETPIXEL   =   0x0001
        GRAPHICS_ROUTINE_BOXF       =   0x0002
        
graphicsRoutines:
        .word graphics1SetPixel,    graphics2SetPixel
        .word graphics4SetPixel,    graphics8SetPixel
        .word graphics16SetPixel,   graphics32SetPixel

        .word graphics1GetPixel,    graphics2GetPixel
        .word graphics4GetPixel,    graphics8GetPixel
        .word graphics16GetPixel,   graphics32GetPixel

        .word graphics1BoxfR,       graphics2BoxfR
        .word graphics4BoxfR,       graphics8BoxfR
        .word graphics16BoxfR,      graphics32BoxfR
        
@ ------------------------------------------------------------------------------
@ graphicsGetOffset(r0->bufferDef, r1=x, r2=y)
@       Returns offset of pixel in r0.
@
.globl graphicsGetOffset
.thumb_func

graphicsGetOffset:
        push {r1, r2, r3, r4, lr}
        ldr r3, [r0, #GRAPHICS_BUFFER_OFFSET]
        ldr r4, [r0, #GRAPHICS_BUFFER_BYTESPERLINE]
        mul r2, r4
        add r3, r2
        ldr r4, [r0, #GRAPHICS_BUFFER_BITSPERPIXEL]
        mul r1, r4
        lsr r1, #3
        add r3, r1
        mov r0, r3
        pop {r1, r2, r3, r4, pc}

                    
@ ------------------------------------------------------------------------------
@ graphicsCallRoutine(r0->bufferDef, r7=routine)
@
.globl graphicsCallRoutine
.thumb_func

graphicsFindRoutine:
        push {r6, lr}
        mov r6, #(4*6)
        mul r7, r6
        ldr r6, [r0, #GRAPHICS_BUFFER_BITSPERPIXEL]
        cmp r6, #32
         bne gcr32
        add r7, #(5*4)
        b gcrdo
gcr32:  cmp r6, #16
         bne gcr16
        add r7, #(4*4)
        b gcrdo
gcr16:  cmp r6, #8
         bne gcr8
        add r7, #(3*4)
        b gcrdo
gcr8:   cmp r6, #4
         bne gcr4
        add r7, #(2*4)
        b gcrdo
gcr4:   cmp r6, #2
         bne gcr2
        add r7, #(1*4)
        b gcrdo
gcr2:
gcrdo:
        ldr r6, =graphicsRoutines
        ldr r7, [r6, r7]
        pop {r6, pc}
                    

@ ------------------------------------------------------------------------------
@ graphicsSetPixel(r0->bufferDef, r1=x, r2=y, r3=c)
@
.globl graphicsSetPixel
.thumb_func

graphicsSetPixel:
        push {r1, r2, r3, r7, lr}
        mov r7, #GRAPHICS_ROUTINE_SETPIXEL        
        bl graphicsFindRoutine
        mov lr, pc
        mov pc, r7    
        pop {r1, r2, r3, r7, pc}


@ ------------------------------------------------------------------------------
@ graphicsGetPixel(r0->bufferDef, r1=x, r2=y)
@       Returns the pixels color in r0
@
.globl graphicsGetPixel
.thumb_func

graphicsGetPixel:
        push {r1, r2, r3, r7, lr}
        mov r7, #GRAPHICS_ROUTINE_GETPIXEL
        bl graphicsFindRoutine
        mov lr, pc
        mov pc, r7    
        pop {r1, r2, r3, r7, pc}


@ ------------------------------------------------------------------------------
@ graphicsBoxf(r0->bufferDef, r1=x, r2=y, r3=width, r4=height, r5=c)
@
.globl graphicsBoxf
.thumb_func

graphicsBoxf:
        push {r1, r2, r3, r4, r5, r7, lr}
        mov r7, #GRAPHICS_ROUTINE_BOXF
        bl graphicsFindRoutine
        ldr r4, [sp, #(7*4)]
        ldr r5, [sp, #(8*4)]
        mov lr, pc
        mov pc, r7
        pop {r1, r2, r3, r4, r5, r7, pc}
        
        .include "graphics1.s"
        .include "graphics2.s"
        .include "graphics4.s"
        .include "graphics8.s"
        .include "graphics16.s"
        .include "graphics32.s"
        
        .arm
        .ltorg

.endif
