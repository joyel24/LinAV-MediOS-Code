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

loop1:

        add r0, pc, #1
        bx r0                   @ switch to thumb
        
        .thumb

        ldr r0, =(lcdInitT+1)
        mov r1, #1
        mov lr, r1
        add lr, pc
@        bx r0

        
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
        ldr r2, =(hex2T+1)
        mov r3, #1
        mov lr, r3
        add lr, pc
        bx r2

        ldr r0, posx
        ldr r1, posy
        mov r2, #20
        mov r3, #15
        ldr r4, cuec
        add r5, r4, #1
        ldr r6, =cuec
        str r5, [r6, #0]
        ldr r6, =(lcdRectfT+1)
        mov r7, #1
        mov lr, r7
        add lr, pc
        bx r6

        @ print the hex value as well?
        
        ldr r0, posx
        ldr r1, posy
        add r1, r1, #4
        add r0, r0, #4
        ldr r2, =hexval
        ldr r3, =0xffff
        ldr r4, =(lcdPutsT+1)
        mov r5, #1
        mov lr, r5
        add lr, pc
        bx r4
        
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
