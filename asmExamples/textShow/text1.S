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

.text

loop1:
        add r0, pc, #1
        bx r0                       @ switch to thumb
        
        .thumb
        
        ldr r0, =(lcdInitT+1)
        mov r1, #1
        mov lr, r1
        add lr, pc
        bx r0
        
        mov r0, #4
        mov r1, #0
        ldr r2, =message1a
        ldr r3, =0x0202
        ldr r4, =(lcdPutsT+1)
        mov r5, #1
        mov lr, r5
        add lr, pc
        bx r4
                
loopr:

        
        ldr r0, =(buttonsGetStatusT+1)
        mov r1, #1
        mov lr, r1
        add lr, pc
        bx r0
 
        mov r1, #0x1
        tst r0, r1
        bne n_up
        ldr r1, saoff
        sub r1, #16
        ldr r2, =saoff
        str r1, [r2]
n_up:   mov r1, #0x20
        tst r0, r1
        bne n_dn
        ldr r1, saoff
        add r1, #16
        ldr r2, =saoff
        str r1, [r2]
n_dn:

        
        ldr r0, saoff
        ldr r1, =caoff
        str r0, [r1]
        mov r0, #18
        ldr r1, =cyoff
        str r0, [r1, #0]
lfop:
        ldr r0, caoff
        ldr r2, =(buildLine+1)
        mov r1, #1
        mov lr, r1
        add lr, pc
        bx r2

        ldr r0, caoff
        add r0, r0, #16
        ldr r1, =caoff
        str r0, [r1, #0]
        
        mov r0, #4
        ldr r1, cyoff
        ldr r2, =messagf1
        ldr r3, =0xffff
        ldr r4, =(lcdPutsT+1)
        mov r5, #1
        mov lr, r5
        add lr, pc
        bx r4
        
        ldr r1, cyoff
        add r1, r1, #6
        ldr r0, =cyoff
        str r1, [r0, #0]
        ldr r0, =(18 + (6*16))
        cmp r1, r0
        blt lfop
        
        b loopr
        
saoff:  .word 0x30a00
caoff:  .word 0
cyoff:  .word 156
        
        .ltorg
        
@ r0->data to read...
buildLine:
        push {lr}
        mov r7, r0

        ldr r0, =messagf1
        mov r1, r7
        ldr r2, =(hex8T+1)
        mov r3, #1
        mov lr, r3
        add lr, pc
        bx r2
        
        mov r6, #8
        ldr r5, =messagf1h
        ldr r4, =messagf1a
loopda:
        ldrh r1, [r7, #0]
        add r7, r7, #2
        
        mov r2, #0xff
        and r2, r2, r1
        cmp r2, #0x20
        bge good1
        mov r2, #0x2e        
good1:  cmp r2, #0x7e
        ble good2
        mov r2, #0x2e
good2:
        strb r2, [r4, #0]       @ store a chr

        mov r2, #0xff
        lsr r1, #8
        and r2, r1
        cmp r2, #0x20
        bge good3
        mov r2, #0x2e        
good3:  cmp r2, #0x7e
        ble good4
        mov r2, #0x2e
good4:
        strb r2, [r4, #1]       @ store a chr
        
        
        mov r0, r5
        ldr r2, =(hex4T+1)
        mov r3, #1
        mov lr, r3
        add lr, pc
        bx r2
        
        add r5, #5
        add r4, #2
        sub r6, #1
        bne loopda
        pop {r0}
        bx r0

        .ltorg
        
messagf1:
        .ascii  "-------- | "
messagf1h:
        .asciz  "---- ---- ---- ---- ---- ---- ---- ---- "
messagf1a:
        .asciz  "................\r\n"

        .align 4
message1a:  .ascii "   -----====#### SYSTEM INFORMATION ####====-----   "            
            .asciz "                                                    "
            
        .align 4
        
.include "../includes/hex.S"
.include "../includes/buttons.S"
.include "../includes/lcd.S"

