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
.include "../includes/av300.S"

.text

loop1:

        add r0, pc, #1
        bx r0                   @ switch to thumb
        
        .thumb

        @
        @ Need to setup UART as 9600, 1 stop, no parity.
        @ 
        
        ldr r0, =0x30302
        ldr r1, =0x173
        strh r1, [r0]           @ 9600 Baud...
        
        
        
        
        callThumb r0, lcdInitT
        
mloop:
        callThumb r1, uartGetchT
        ldr r1, =(-1)
        cmp r0, r1
        beq mloop               @ no input!

        callThumb r2, visorConvertToAscii
        cmp r0, #0
         beq mloop               @ key up or something prolly?
        
@        mov r1, r0
@        ldr r0, =msg1h
@        callThumb r4, hex2T

@        callThumb1 r4, terminalPutsT, msg1

        callThumb r4, terminalPutchT
        
        b mloop

msg1:   .ascii "Byte "
msg1h:  .asciz "xx\n"
        
        .align
        
.include "../includes/uart.S"
.include "../includes/terminal.S"
.include "../includes/hex.S"
.include "../includes/visorKeyboard.S"
