@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@
@ Date:     24/01/2004
@ Author:   By DoggerMoore
@
@

.include "../includes/common.S"
.include "../includes/av300.S"

.text
        add r0, pc, #1
        bx r0                                    @ switch to thumb mode

        .thumb

        callThumb r1, lcdInitT                   @ Initialise the LCD

        mov r0, #4
        mov r1, #12
        ldr r2, =msgHello
        ldr r3, =0x0404
        callThumb r5, lcdPutsT
        
@ ---------------------------------------------- @ Copy background image
        ldr r3, =lcdVideoBuffer2
        ldr r4, =sprite
        ldr r5, =(640*480)
lcop:   mov r0, #0
        mov r1, #0
        mov r2, #0
        ldrb r0, [r4]
        ldrb r1, [r4, #1]
        ldrb r2, [r4, #2]
        add r4, #3
        callThumb r6, colorRGB2YCbCrT
        callThumb r6, colorPackYCbCrT
        str r0, [r3]
        add r3, #4
        sub r5, #1
        bne lcop   

        callThumb2 r3, osdSetMainConfigT, 0x14, 0xc1    @ 1100 
        callThumb2 r3, osdSetBackConfigT, 0x28, 0x03    @ 0000 0011
        callThumb2 r4, osdSetBackSizeT, 640, 240
        
        bl fadeIn
        
iloop:

        callThumb r1, buttonsGetStatusT
        
        ldr r2, =posy
        ldr r3, [r2]
        mov r1, #1
        tst r0, r1
         bne nUP
        cmp r3, #0
         beq nUP
        sub r3, #1
nUP:    mov r1, #0x20
        tst r0, r1
         bne nDN
        ldr r4, =240
        cmp r3, r4
         beq nDN
        add r3, #1
nDN:    str r3, [r2]

        ldr r2, =posx
        ldr r3, [r2]
        mov r1, #0x2
        tst r0, r1
         bne nLT
        cmp r3, #0
         beq nLT
        sub r3, #1
nLT:    mov r1, #0x10
        tst r0, r1
         bne nRT
        ldr r4, =320
        cmp r3, r4
         beq nRT
        add r3, #1
nRT:    str r3, [r2]
        
        ldr r1, =0x100
        tst r0, r1
         bne nM1
        bl fadeIn     
nM1:    ldr r1, =0x200
        tst r0, r1
         bne nM2
        bl fadeOut
nM2:

        ldr r0, posy
        ldr r1, =640
        mul r0, r1
        ldr r1, posx
        add r0, r1
        lsl r0, #2
        ldr r1, =lcdVideoBuffer2
        add r0, r1

        callThumb r2, osdSetBackOffsetT

        ldr r0, =0x400
wait1:  sub r0, #1
        bne wait1

        b iloop
@ ------------------------------------------------------------------------------
        
fadeIn: push {r0, lr}        
        mov r6, #8               
        mov r5, #0xf9                                   @ starts at f9
fdloop:
        mov r1, r5
        callThumb1 r3, osdSetMainConfigT, 0x14 

        ldr r0, =0x10000
fwait:  sub r0, #1
        cmp r0, #0
         bne fwait
        sub r5, #8
         
        sub r6, #1
        cmp r6, #0
         bne fdloop
        pop {r0, pc}

        
fadeOut:push {r0, lr}        
        mov r6, #8               
        mov r5, #0xc1                                   @ starts at c1
fdloop2:
        mov r1, r5
        callThumb1 r3, osdSetMainConfigT, 0x14 

        ldr r0, =0x10000
fwait2: sub r0, #1
        cmp r0, #0
         bne fwait2
        add r5, #8
         
        sub r6, #1
        cmp r6, #0
         bne fdloop2
        pop {r0, pc}
        
@ ------------------------------------------------------------------------------
        .align 4        

        .ltorg

trVal:      .word 0
        
posx:       .word 0
posy:       .word 0

msgHello:   .ascii "bigImage (c)2004 DoGgEr. Loading... Get ready for   "
            .asciz "the fade! Just demonstrates a bit more of the OSD..."
        .align 4
.include "../includes/lcd.S"
.include "../includes/buttons.S"
.include "../includes/color.S"

sprite:
.incbin     "image.rgb"
