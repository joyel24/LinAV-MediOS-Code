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

@.include "../includes/common.S"

.ifndef visorKeyboardInc
visorKeyboardInc = 1

.text
        .thumb

@ visorConvertToAscii r0=scan code
@                     r0=ascii, or 0 if not translatable.
.globl visorConvertToAscii
.thumb_func
visorConvertToAscii:
        push {lr}
        push {r1, r2, r3, r4}
        mov r1, r0
        mov r0, #0
        lsl r1, #24
        lsr r1, #24

        mov r2, #0x1a
        ldr r3, =kShift
        callThumb r5, visorUpdateShift
        cmp r4, #1
         beq visor1
        
        cmp r1, #0x80
         bge visor1
        mov r0, #0x0a
        cmp r1, #0x3b
         beq visor1
        ldr r2, =xlatTable
        ldr r3, kShift
        cmp r3, #0
         beq visorn
        ldr r2, =xlatTableS        
visorn:
        ldrb r0, [r2, r1]
visor1:
        pop {r1, r2, r3, r4}
        pop {r1}
        bx r1

@ visorUpdateShift (0, key, cmpVal, ->var)
@           Returns in r4 1 if its the shift
.globl visorUpdateShift
.thumb_func
visorUpdateShift:
        push {r0}
        mov r4, #0
        cmp r1, r2
         bne visorn1
        mov r0, #1
        str r0, [r3]
        mov r4, #1
        b visorn2
visorn1:mov r0, #0x80
        orr r2, r0
        cmp r1, r2
         bne visorn2
        mov r0, #0
        str r0, [r3]
        mov r4, #1        
visorn2:
        pop {r0}
        bx lr

        
        .ltorg

xlatTable:
        .ascii "5678ghij....----"
        .ascii "90abstuv....----"
        .ascii "opqrwxyz....----"
        .ascii "-=[]/.......----"
        .ascii "cdef'`      ----"
        .ascii "1234klmn\\;,.----"
xlatTableS:
        .ascii "%^&*GHIJ....----"
        .ascii "()ABSTUV....----"
        .ascii "OPQRWXYZ....----"
        .ascii "_+{}?.......----"
        .ascii "CDEF\"~      ----"
        .ascii "!@#$KLMN|:<>----"

kCtrl:  .word 0
kShift: .word 0
kAlt:   .word 0
kCaps:  .word 0
kCmd:   .word 0

        
        .arm
.endif
