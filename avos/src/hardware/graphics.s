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
@ Whats done?       Destinateion bits per pixel
@              |  1 |  2 |  4 |  8 | 16 | 32 |
@ -------------+----+----+----+----+----+----+
@ setPixel     |    |    |    |  Y |  Y |  Y |
@ getPixel     |    |    |    |  Y |  Y |  Y |
@ boxf         |    |    |    |  Y |  Y |  Y |
@ sprite1      |    |    |    |    |  Y |  Y |
@ sprite2      |    |    |    |    |  Y |  Y |
@ sprite4      |    |    |    |    |  Y |  Y |
@ sprite8      |    |    |    |    |  Y |  Y |
@
@ sprite16     |    |    |    |    |  Y |    |
@ sprite32     |    |    |    |    |    |  Y |
@
@
@ NB 16bit per pixel is actually kindof 8 bit per pixel.
@ it's just 2* 8 bit pixels side by side. This is used when hardware zoom is
@ set to x2
@ Also, 32 bit is actually likewise 2 pixels. Y0 Cb Y1 Cr.
@ This is again used for hardware zoom x2/x4.

.macro switchThumb
        .arm
        add ip, pc, #1
        bx ip
        .thumb
.endm


.ifndef graphicsInc
graphicsInc = 1

.text
        GRAPHICS_BUFFER_OFFSET      =   0x0000      @ Offset of buffer
        GRAPHICS_BUFFER_BYTESPERLINE=   0x0004      @ Bytes per scanline
        GRAPHICS_BUFFER_WIDTH       =   0x0008      @ Width in pixels
        GRAPHICS_BUFFER_HEIGHT      =   0x000c      @ Height in pixels
        GRAPHICS_BUFFER_BITSPERPIXEL=   0x0010      @ Bits per pixel
        GRAPHICS_BUFFER_BPPSHIFT    =   0x0014
        GRAPHICS_BUFFER_TRANSPARENT =   0x0018
        GRAPHICS_BUFFER_PALLETTE1   =   0x001c      @ ->Pallette for 1bit        
        GRAPHICS_BUFFER_PALLETTE2   =   0x0020      @ ->Pallette for 2bit
        GRAPHICS_BUFFER_PALLETTE4   =   0x0024      @ ->Pallette for 4bit
        GRAPHICS_BUFFER_PALLETTE8   =   0x0028      @ ->Pallette for 8bit
        GRAPHICS_BUFFER_PALLETTE16  =   0x002c      @ ->Pallette for 16bit
        GRAPHICS_BUFFER_PALLETTE32  =   0x0030      @ ->Pallette for 32bit
        GRAPHICS_BUFFER_SIZE        =   0x0034
        
        GRAPHICS_ROUTINE_SETPIXEL   =   0x0000
        GRAPHICS_ROUTINE_GETPIXEL   =   0x0001
        GRAPHICS_ROUTINE_BOXF       =   0x0002
        GRAPHICS_ROUTINE_SPRITE     =   0x0003

        .align 4
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

        .word graphics1Sprite,      graphics2Sprite
        .word graphics4Sprite,      graphics8Sprite
        .word graphics16Sprite,     graphics32Sprite

@ ------------------------------------------------------------------------------
@ graphicsRGB2Packed(r0=r, r1=g, r2=b)
@       Returns Packed CrYCb value.
@
.globl graphicsRGB2PackedA
graphicsRGB2PackedA:
        switchThumb
.globl graphicsRGB2Packed
.thumb_func

graphicsRGB2Packed:
        push {r4, r5, r6}
        mov r5, r0
        ldr r4, =306
        mul r5, r4          @ r5 = r*306
        mov r3, r1
        ldr r4, =601
        mul r3, r4
        add r5, r3          @ r5 = r*306 + g*601
        mov r3, r2
        ldr r4, =117
        mul r3, r4
        add r5, r3          @ r5 = r*306 + g*601 + b*117
        lsr r5, #10         @ int y = (306*r + 601*g + 117*b) >> 10;
        lsl r5, #8
        mov r6, r5          @ Ready in r6
        
        mov r5, r0
        ldr r4, =173
        mul r5, r4          @ r5 = r*173
        mov r3, r1
        ldr r4, =339
        mul r3, r4
        add r5, r3          @ r5 = r*173 + g*339
        mov r3, r2
        ldr r4, =256
        add r3, r4
        lsl r3, #9          @ r3 = b*512
        sub r3, r5          @ r3 = b*512 - r*173 - g*339
        lsr r3, #10         @ int cb = ((-173*r -339*g + 512*b) >> 10) + 128;
        orr r6, r3

        mov r5, r1
        ldr r4, =429
        mul r5, r4
        mov r3, r2
        ldr r4, =83
        mul r3, r4
        add r5, r3
        ldr r4, =256
        add r0, r4
        lsl r0, #9
        sub r0, r5
        lsr r0, #10         @ int cr = (512*r - 429*g - 83*b) >> 10) + 128;
        lsl r0, #16
        orr r0, r6
        pop {r4, r5, r6}
        bx lr
        
@ ------------------------------------------------------------------------------
@ graphicsGetOffset(r0->bufferDef, r1=x, r2=y)
@       Returns offset of pixel in r0.
@
.thumb_func

graphicsGetOffset:
        push {r1, r2, r3, r4}
        ldr r3, [r0, #GRAPHICS_BUFFER_OFFSET]
        ldr r4, [r0, #GRAPHICS_BUFFER_BYTESPERLINE]
        mul r2, r4
        add r3, r2
        ldr r4, [r0, #GRAPHICS_BUFFER_BITSPERPIXEL]
        mul r1, r4
        lsr r1, #3
        add r3, r1
        mov r0, r3
        pop {r1, r2, r3, r4}
        bx lr

                    
@ ------------------------------------------------------------------------------
@ graphicsFindRoutine(r0->bufferDef, r7=routine)
@
.thumb_func

graphicsFindRoutine:
        push {r6}
        mov r6, #(4*6)
        mul r7, r6
        ldr r6, [r0, #GRAPHICS_BUFFER_BPPSHIFT]
        lsl r6, #2
        add r7, r6
        ldr r6, =graphicsRoutines
        ldr r7, [r6, r7]
        pop {r6}
        bx lr
  

@ ------------------------------------------------------------------------------
@ graphicsSetPixel(r0->bufferDef, r1=x, r2=y, r3=c)
@
.globl graphicsSetPixelA
graphicsSetPixelA:
        switchThumb
.globl graphicsSetPixel
.thumb_func

graphicsSetPixel:
        push {r1, r2, r3, r7, lr}
        mov r7, #GRAPHICS_ROUTINE_SETPIXEL        
        bl graphicsFindRoutine
        mov lr, pc
        mov pc, r7
        pop {r1, r2, r3, r7}
        pop {r1}
        bx r1


@ ------------------------------------------------------------------------------
@ graphicsGetPixel(r0->bufferDef, r1=x, r2=y)
@       Returns the pixels color in r0
@
.globl graphicsGetPixelA
graphicsGetPixelA:
        switchThumb
.globl graphicsGetPixel
.thumb_func

graphicsGetPixel:
        push {r1, r2, r3, r7, lr}
        mov r7, #GRAPHICS_ROUTINE_GETPIXEL
        bl graphicsFindRoutine
        mov lr, pc
        mov pc, r7
        pop {r1, r2, r3, r7}
        pop {r1}
        bx r1


@ ------------------------------------------------------------------------------
@ graphicsBoxf(r0->bufferDef, r1=x, r2=y, r3=width, r4=height, r5=c)
@
.globl graphicsBoxfA
graphicsBoxfA:
        switchThumb
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
        pop {r1, r2, r3, r4, r5, r7}
        pop {r1}
        bx r1


@ ------------------------------------------------------------------------------
@ graphicsSprite(r0->bufferDefDest, r1=x, r2=y, r3=bufferDefSrc)
@
.globl graphicsSpriteA
graphicsSpriteA:
        switchThumb
.globl graphicsSprite
.thumb_func

graphicsSprite:
        push {r1, r2, r3, r4, r5, r7, lr}
        mov r7, #GRAPHICS_ROUTINE_SPRITE
        bl graphicsFindRoutine
        mov lr, pc
        mov pc, r7
        pop {r1, r2, r3, r4, r5, r7}
        pop {r1}
        bx r1
        
        
@ ------------------------------------------------------------------------------
@ graphicsString(r0->bufferDefDest, r1=x, r2=y, r3=bufferDefSrc,
@                r4->fontLUT, r5=dx, r6=dy, r7->String)
@
.globl graphicsStringA
graphicsStringA:
        switchThumb
.globl graphicsString
.thumb_func

graphicsString:
        push {r4, r5, r6, r7, lr}
        ldr r4, [sp, #(5*4)]
        ldr r5, [sp, #(6*4)]
        ldr r6, [sp, #(7*4)]
        ldr r7, [sp, #(8*4)]
        bl graphicsStringR
        pop {r4, r5, r6, r7}
        pop {r1}
        bx r1
        

graphicsStringR:
        push {r1, r2, r7, lr}        
        
gsloop: push {r6}
        mov r6, #0
        ldrb r6, [r7]
        cmp r6, #0
         beq gsdone
         
        lsl r6, #2
        ldr r6, [r4, r6]
        cmp r6, #0
         beq gsnochr
        str r6, [r3, #GRAPHICS_BUFFER_OFFSET]
        mov r6, r1
        bl graphicsSprite
        mov r1, r6
gsnochr:
        pop {r6}
        add r1, r5
        add r2, r6

        add r7, #1
        b gsloop
gsdone: pop {r6}
        pop {r1, r2, r7}
        pop {r1}
        bx r1

        .arm
        .ltorg

.endif
