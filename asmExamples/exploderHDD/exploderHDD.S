@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@ exploderHDD
@
@ Date:     17/01/2004
@ Author:   By DoggerMoore
@
@
        .include "../includes/common.S"
        .include "../includes/av300.S"

.text

        add r0, pc, #1
        bx r0                       @ switch to thumb

        .thumb

        callThumb r0, lcdInitT        
           
        
        mov r0, #4
        mov r1, #0
        ldr r2, =message1a
        ldr r3, =0xfcfc
        callThumb r4, lcdPutsT
        
        mov r0, #4
        mov r1, #12
        ldr r2, =message1c
        ldr r3, =0xfafa
        callThumb r4, lcdPutsT
        
        mov r0, #4
        ldr r1, =36*6
        ldr r2, =message2a
        ldr r3, =0xfbfb
        callThumb r4, lcdPutsT

        callThumb r4, ataInitT
loopr:
        callThumb r0, buttonsGetStatusT        

        ldr r1, =0x777
        eor r0, r1
        mov r6, r0              @ to detect menu3
        ldr r1, =0x333
        and r0, r1
        
        ldr r7, saoff

        cmp r0, #0x1
        bne notup
        sub r7, #0x1
notup:  cmp r0, #0x20
        bne notdown
        add r7, #0x1
notdown:cmp r0, #0x2
        bne notleft
        sub r7, #0x10
notleft:cmp r0, #0x10
        bne notright
        add r7, #0x10
notright:

        ldr r1, =0x101
        ldr r2, =0x100
        cmp r0, r1
        bne not1up
        sub r7, r2
not1up:
        ldr r1, =0x120
        cmp r0, r1
        bne not1down
        add r7, r2
not1down:

        ldr r2, =0x1000
        ldr r1, =0x102
        cmp r0, r1
        bne not1left
        sub r7, r2
not1left:
        ldr r1, =0x110
        cmp r0, r1
        bne not1right
        add r7, r2
not1right:

        ldr r2, =0x10000
        ldr r1, =0x201
        cmp r0, r1
        bne not2up
        sub r7, r2
not2up: ldr r1, =0x220
        cmp r0, r1
        bne not2down
        add r7, r2
not2down:

        ldr r2, =0x100000
        ldr r1, =0x202
        cmp r0, r1
        bne not2left
        sub r7, r2
not2left:
        ldr r1, =0x210
        cmp r0, r1
        bne not2right
        add r7, r2
not2right:

        cmp r6, #0x40
        bne notmenu3
        ldr r7, intaddrp
        add r5, r7, #4
        ldr r1, =intaddrp
        cmp r5, r1
        blt noov
        ldr r5, =intaddrs
noov:   str r5, [r1]
        ldr r7, [r7]
notmenu3:
        
        cmp r7, #0
        bge okr7
        mov r7, #0
okr7:

        ldr r1, =saoff
        str r7, [r1]


        ldr r0, =message1bh
        mov r1, r7
        callThumb r2, hex8T
        
        mov r0, #4
        mov r1, #6
        ldr r2, =message1b
        ldr r3, =0xfbfb
        callThumb r4, lcdPutsT

        ldr r0, =bufferx
        ldr r1, saoff
        mov r2, #1
        callThumb r4, ataReadLBAT
        
        
        
        mov r5, #18                 @ r5 = y position
        mov r6, #0                  @ current offset
lfop:
        mov r0, r6
        callThumb r2, buildLine

        mov r0, #4
        mov r1, r5
        ldr r2, =messagf1
        ldr r3, =0xffff
        callThumb r4, lcdPutsT

        add r5, #6        

        add r6, #0x10
        ldr r0, =0x200
        cmp r6, r0
        blt lfop
        
        b loopr
        
        .ltorg
        
@ r0->data to read...
buildLine:
        push {lr}
        push {r4, r5, r6, r7}
        mov r7, r0

        ldr r0, =messagf1
        mov r1, r7
        callThumb r2, hex2T

        ldr r6, =bufferx
        add r7, r6
        
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
        callThumb r3, hex4T
        
        add r5, #4
        add r4, #2
        sub r6, #1
        bne loopda
        pop {r4, r5, r6, r7}
        pop {r0}
        bx r0

        .ltorg
        
messagf1:
        .ascii  "-- "
messagf1h:
        .ascii     "-------------------------------- "
messagf1a:
        .asciz  "................"

        .align 4
message1a:  .asciz " ------=====##### E X P L O D E R #####=====------  "            

message1b:  .ascii "LBA: "
message1bh: .asciz "xxxxxxxx"
message1c:  .asciz "++ -00--02--04--06--08--0A--0C--0E- 0123456789ABCDEF"

message2a:  .ascii "     UP-00000001 DN+00000001 LT-00000010 RT+00000010"
            .ascii "F1+  UP-00000100 DN+00000100 LT-00001000 RT+00001000"
            .asciz "F2+  UP-00010000 DN+00010000 LT-00100000 RT+00100000"
            
        .align 4
        
saoff:  .word 1        
intaddrs:   .word 0x00000001
            .word 0x00000010
intaddrp:   .word intaddrs

        
.include "../includes/hex.S"
.include "../includes/buttons.S"
.include "../includes/lcd.S"
.include "../includes/ata.S"
.include "../includes/color.S"

bufferx:
