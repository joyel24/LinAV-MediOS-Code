@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@ text1
@
@ Date:     06/01/2004
@ Author:   By DoggerMoore
@
@
        .include "../includes/common.S"

.text

loop1:
        add r0, pc, #1
        bx r0                       @ switch to thumb
        
        .thumb
       
        ldr r0, =bufferx
        
        callThumb r0, lcdInitT

        callThumb1 r4, terminalPutsT, msg1
        
        callThumb r4, ataInitT

@        callThumb1 r4, ataIdentifyT, bufferx
        
        ldr r0, =bufferx
        mov r1, #1
        mov r2, #1
        callThumb r4, ataReadLBAT

        ldr r3, =256
        ldr r2, =bufferx
loopxf: ldrh r1, [r2]
        add r2, #2
        callThumb1 r4, hex4T, msg2h
        callThumb1 r4, terminalPutsT, msg2
        callThumb1 r4, uartOutsT, msg2
        sub r3, #1
        bne loopxf

loopr:        
        b loopr
        
        .ltorg

msg1:   .asciz "Initialising ATA...\n"
msg2:   .ascii ":"
msg2h:  .asciz "xxxx"

        .align
.include "../includes/hex.S"
.include "../includes/buttons.S"
.include "../includes/lcd.S"
.include "../includes/ata.S"
.include "../includes/terminal.S"
.include "../includes/uart.S"

bufferx:
        .word 0

