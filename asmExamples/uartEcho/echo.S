@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@ echo
@
@ Date:     04/01/2004
@ Author:   By DoggerMoore
@
@
.include "../includes/common.S"

.text

        add r0, pc, #1
        bx r0                   @ switch to thumb
        
        .thumb

        b overb
        .space 0x46, 0
        .asciz "CJBMVERSION2000"
        .space 0x20, 0
overb:

        callThumb r1, lcdInitT

        ldr r7, =0x30500
        mov r6, #0x40
ldump:
        mov r0, r7
        callThumb r1, buildLine
        callThumb1 r1, uartOutsT, messagf1
        add r7, #0x10
        sub r6, #1
        bne ldump
        
        
mloop:
        callThumb r1, uartGetchT
        ldr r1, =(-1)
        cmp r0, r1
        beq mloop               @ no input!
        
        callThumb r1, uartOutchT

        b mloop

.include "../includes/uart.S"
.include "../includes/lcd.S"
.include "../includes/hex.S"

        .thumb

@ r0->data to read...
buildLine:
        push {lr}
        push {r1, r2, r3, r4, r5, r6, r7}
        mov r7, r0

        ldr r0, =messagf1
        mov r1, r7
        callThumb r2, hex2T
        
        mov r6, #8
        ldr r5, =messagf1h
        ldr r4, =messagf1a
loopda:
        ldrh r1, [r7, #0]
        add r7, r7, #2
        
        mov r2, #0xff
        and r2, r1
        cmp r2, #0x20
        bge good1
        mov r2, #0x2e        
good1:  cmp r2, #0x7e
        ble good2
        mov r2, #0x2e
good2:
        strb r2, [r4, #0]       @ store a chr

        mov r2, #0xff
        mov r3, r1
        lsr r3, #8
        and r2, r3
        cmp r2, #0x20
        bge good3
        mov r2, #0x2e        
good3:  cmp r2, #0x7e
        ble good4
        mov r2, #0x2e
good4:
        strb r2, [r4, #1]       @ store a chr
                
        mov r0, r5
        callThumb r2, hex4T
        
        add r5, #4
        add r4, #2
        sub r6, #1
        bne loopda
        pop {r1, r2, r3, r4, r5, r6, r7}
        pop {r0}
        bx r0

messagf1:
        .ascii  "-- "
messagf1h:
        .ascii     "-------------------------------- "
messagf1a:
        .asciz  "................\n"

