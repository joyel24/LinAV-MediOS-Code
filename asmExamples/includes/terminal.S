@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@ Terminal
@
@
@ Date:     09/01/2004
@ Author:   By DoggerMoore
@
@ terminalSetPosT r0=x, r1=y
@ terminalPutchT r0=chr
@ terminalSetColorFT r0=color
@

.text
        newLineCharacter    =   0x0a

        fontWidth           =   6
        fontHeight          =   6

        terminalWidth       =   52
        terminalHeight      =   40

        .thumb

@ terminalSetColorFT r0=color
@
terminalSetColorFT:
        push {r1}
        ldr r1, =terminalColor
        str r0, [r1]
        pop {r1}
        bx lr

@ terminalSetPosT r0=x, r1=y
@
terminalSetPosT:
        push {r2}
        ldr r2, =terminalPosX
        str r0, [r2]
        ldr r2, =terminalPosY
        str r1, [r2]
        pop {r2}
        bx lr

@ terminalPutchT r0=chr
@
terminalPutchT:
        push {r1, r2, r3, r4, lr}
        
        ldr r1, =0xff
        and r0, r1
        
        cmp r0, #newLineCharacter
        beq xover
        
        mov r2, r0
        ldr r0, terminalPosX
        ldr r1, =fontWidth
        mul r0, r1
        add r0, #4                      @ x offset
        
        ldr r1, terminalPosY
        ldr r3, =fontHeight
        mul r1, r3
        
        ldr r3, terminalColorF
        callThumb r4, lcdPutchT         @ print the chr
        
        @ now update stuff...
        
        ldr r0, terminalPosX
        add r0, #1
        ldr r1, =terminalWidth
        cmp r0, r1
        bge xover
        ldr r1, =terminalPosX
        str r0, [r1]
        b donetp
xover:  mov r0, #0
        ldr r1, =terminalPosX
        str r0, [r1]

        ldr r0, terminalPosY
        add r0, #1
        ldr r1, =terminalHeight
        cmp r0, r1
        bge yover
        ldr r1, =terminalPosY
        str r0, [r1]
        b donetp
yover:
        mov r0, #6
        ldr r1, terminalColorB
        callThumb r2, lcdScrollUpT  @ scroll up the display.

donetp:
        pop {r1, r2, r3, r4}
        pop {r0}
        bx r0

        .ltorg
        .arm
        
terminalPosX:   .word 0
terminalPosY:   .word 0
terminalColorF: .word 0xffff
terminalColorB: .word 0x0000
