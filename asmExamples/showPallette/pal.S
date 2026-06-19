@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@ pallette
@
@ Date:     04/01/2004
@ Author:   By DoggerMoore
@
@

.text
        .include "../includes/common.S"
        .include "../includes/av300.S"
loop1:

        add r0, pc, #1
        bx r0                   @ switch to thumb
        
        .thumb

        callThumb r0, lcdInitT

        callThumb2 r3, osdSetMainConfigT, 0x14, 0x10d9      

        mov r5, #0
lpal:
        mov r0, r5
        mov r1, r5
        mov r2, r5
        callThumb r3, colorRGB2YCbCrT
        
        mov r3, r5
        callThumb r4, osdSetPalletteColorT

        ldr r4, =0x100
        add r5, #1
        cmp r4, r5
        bne lpal
        
        mov r0, #0
        ldr r1, =posy
        str r0, [r1, #0]
loowy:
        
        mov r0, #0
        ldr r1, =posx
        str r0, [r1, #0]
loowx:
        ldr r1, cuec
        ldr r0, =hexval
        callThumb r2, hex2T

        ldr r0, posx
        ldr r1, posy
        mov r2, #20
        mov r3, #15
        ldr r4, cuec
        add r5, r4, #1
        ldr r6, =cuec
        str r5, [r6, #0]
        callThumb r6, lcdRectfT

        @ print the hex value as well?
        
        ldr r0, posx
        ldr r1, posy
        add r1, r1, #4
        add r0, r0, #4
        ldr r2, =hexval
        ldr r3, =0xffff
        callThumb r4, lcdPutsT
        
        @ done hex bit...

        ldr r0, posx
        add r0, #20
        ldr r1, =posx
        str r0, [r1, #0]
        ldr r1, =320
        cmp r0, r1
         blt loowx

        ldr r0, posy
        add r0, r0, #15
        ldr r1, =posy
        str r0, [r1, #0]
        cmp r0, #240
         blt loowy
        
loopb:        
        b loopb

hexval: .asciz  "xx"

        .align 4
posx:   .word 0
posy:   .word 0
cuec:   .word 0

.include "../includes/lcd.S"
.include "../includes/hex.S"
.include "../includes/color.S"
