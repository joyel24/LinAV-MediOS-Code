@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@
@ Date:     17/01/2004
@ Author:   By DoggerMoore
@

        .include "../includes/common.S"
.text
        add r0, pc, #1
        bx r0
        
        .thumb

        callThumb r0, lcdInitT

        ldr r3, =lcdVideoBuffer2
        ldr r4, =sprite
        ldr r5, =(320*240)
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
        
        callThumb2 r3, osdSetBackConfigT, 0x14, 0x03
        callThumb2 r3, osdSetMainConfigT, 0x14, 0xd9      

loopb:
        callThumb r1, buttonsGetStatusT

        ldr r1, posx
        ldr r2, =oposx
        str r1, [r2]
        ldr r1, posy
        ldr r2, =oposy
        str r1, [r2]
        ldr r2, =posx
        ldr r3, =posy
        
        mov r1, #0x1
        tst r0, r1
        bne n_up
        ldr r1, posy
        cmp r1, #2
        ble n_up
        sub r1, #1
        str r1, [r3]
n_up:   mov r1, #0x20
        tst r0, r1
        bne n_dn
        ldr r1, posy
        cmp r1, #237
        bge n_dn
        add r1, #1
        str r1, [r3]
n_dn:   mov r1, #0x02
        tst r0, r1
        bne n_lf
        ldr r1, posx
        cmp r1, #2
        ble n_lf
        sub r1, r1, #1
        str r1, [r2]
n_lf:   mov r1, #0x10
        tst r0, r1
        bne n_rt
        ldr r1, posx
        ldr r4, =318
        cmp r1, r4
        bge n_rt
        add r1, r1, #1
        str r1, [r2]
n_rt:   ldr r1, =0x100
        tst r0, r1
        bne n_m1
        mov r1, #0
        ldr r2, =drawing
        str r1, [r2]
n_m1:   ldr r1, =0x200
        tst r0, r1
        bne n_m2
        mov r1, #1
        ldr r2, =drawing
        str r1, [r2]
n_m2:   ldr r1, =0x040
        tst r0, r1
        bne n_m3
        ldr r2, backon
        mov r3, #1
        eor r2, r3
        ldr r1, =backon
        str r2, [r1]
        cmp r2, #0
        bne b_on
        
        callThumb2 r3, osdSetBackConfigT, 0x14, 0x03
        callThumb2 r3, osdSetMainConfigT, 0x14, 0xd9      
        
        b wfn
b_on:
        
        callThumb2 r3, osdSetBackConfigT, 0x14, 0x02
        callThumb2 r3, osdSetMainConfigT, 0x14, 0xf9      
wfn:    callThumb r3, buttonsGetStatusT
        ldr r1, =0x777
        cmp r0, r1
         bne wfn                @ wait for no button...

n_m3:

        ldr r0, posx
        ldr r1, oposx
        cmp r0, r1
        bne hasmove
        ldr r0, posy
        ldr r1, oposy
        cmp r0, r1
        bne hasmove
                            @ Hasn't moved!
        b loopb
hasmove:       
        ldr r0, drawing
        cmp r0, #1
        beq amdraw
        ldr r0, oposx 
        ldr r1, oposy
        ldr r2, savedp
        callThumb r3, lcdPlotT
        b nodraw
amdraw:
        ldr r0, oposx 
        ldr r1, oposy
        ldr r2, pencolor
        callThumb r3, lcdPlotT
nodraw:
        ldr r0, posx
        ldr r1, posy
        callThumb r2, lcdGetPixelT
        ldr r1, =savedp
        str r0, [r1]      @ save the pixel for later...

        ldr r0, posx
        ldr r1, posy
        ldr r2, =0xffff
        callThumb r3, lcdPlotT

        ldr r2, =0x4000
del1:   sub r2, #1
        bne del1

        b loopb

        
        .ltorg        
        .align 4
posx:   .word 160
posy:   .word 120

oposx:  .word 160
oposy:  .word 120

pencolor: .word 0x0101

backon: .word 0
drawing:.word 0             @ 1=draw
                            @ 0=move
savedp: .word 0x0000        @ saved pixel.

        
.include "../includes/buttons.S"
.include "../includes/lcd.S"
.include "../includes/color.S"

sprite:
.incbin     "image.rgb"
