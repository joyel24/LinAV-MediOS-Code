@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@ visorKeyboard
@
@ Date:     15/01/2004
@ Author:   By DoggerMoore
@
@

.include "../includes/common.S"

.ifndef visorKeyboardInc
visorKeyboardInc = 1

.text
        .thumb

@ visorConvertToAscii r0=scan code
@                     r0=ascii, or 0 if not translatable.
visorConvertToAscii:
        push {r1, r2}
        mov r1, r0
        mov r0, #0
        lsl r1, #24
        lsr r1, #24
        cmp r1, #0x80
         bge ret1
        mov r0, #0x0a
        cmp r1, #0x3b
         beq ret1
        ldr r2, =xlatTable
        ldrb r0, [r2, r1]
ret1:
        pop {r1, r2}
        bx lr

        .ltorg

xlatTable:
        .ascii "5678ghij....----"
        .ascii "90abstuv....----"
        .ascii "opqrwxyz....----"
        .ascii "-=[]/.......----"
        .ascii "cdef'~      ----"
        .ascii "1234klmn|;,.----"
        
        .arm
.endif
