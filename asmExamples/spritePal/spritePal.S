@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@ spritePal
@
@ Date:     04/01/2004
@ Author:   By DoggerMoore
@
@
.include "../includes/common.S"
.include "../includes/av300.S"

.text

        add r0, pc, #1
        bx r0                   @ switch to thumb
        
        .thumb

        callThumb r0, lcdInitT

        callThumb2 r3, osdSetMainConfigT, 0x14, 0x10d9      

        ldr r5, =sprite
        mov r6, #0
lpal:
        mov r0, #0
        mov r1, #0
        mov r2, #0
        ldrb r0, [r5]
        ldrb r1, [r5, #1]
        ldrb r2, [r5, #2]
        add r5, #3
        callThumb r4, colorRGB2YCbCrT
        mov r3, r6
        callThumb r4, osdSetPalletteColorT

        ldr r4, =0x100
        add r6, #1
        cmp r4, r6
         bne lpal
        
        mov r0, #0
        mov r1, #0
        ldr r2, =320
        ldr r3, =240
        ldr r4, =(sprite+768)
        callThumb r5, lcdSpriteBT

mloop:
        b mloop

.include "../includes/lcd.S"
.include "../includes/terminal.S"
.include "../includes/color.S"

sprite:
.incbin "avos.map"

