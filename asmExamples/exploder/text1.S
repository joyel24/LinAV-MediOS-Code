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
        
        ldr r0, =(lcdInitT+1)
        mov r1, #1
        mov lr, r1
        add lr, pc
        bx r0
   
        ldr r3, =lcdVideoBuffer2
        ldr r4, =sprite
        ldr r5, =(320*240)
lcop:   mov r0, #0
        mov r1, #0
        mov r2, #0
        ldrb r0, [r4]
        ldrb r1, [r4, #1]
        ldrb r2, [r4, #2]
        add r4, #3
        callThumb r6, colorRGB2YCbCrT
        callThumb r6, colorPackYCbCrT

        str r0, [r3]
        
        add r3, #4
        sub r5, #1
        bne lcop   
        
        
        mov r0, #4
        mov r1, #0
        ldr r2, =message1a
        ldr r3, =0xfcfc
        ldr r4, =(lcdPutsT+1)
        mov r5, #1
        mov lr, r5
        add lr, pc
        bx r4

        mov r0, #4
        mov r1, #12
        ldr r2, =message1c
        ldr r3, =0xfafa
        ldr r4, =(lcdPutsT+1)
        mov r5, #1
        mov lr, r5
        add lr, pc
        bx r4
        
        mov r0, #4
        ldr r1, =36*6
        ldr r2, =message2a
        ldr r3, =0xfbfb
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

        ldr r1, =0x777
        eor r0, r1
        mov r6, r0              @ to detect menu3
        ldr r1, =0x333
        and r0, r1
        
        ldr r7, saoff

        cmp r0, #0x1
        bne notup
        sub r7, #0x10
notup:  cmp r0, #0x20
        bne notdown
        add r7, #0x10
notdown:cmp r0, #0x2
        bne notleft
        sub r7, #0x2
notleft:cmp r0, #0x10
        bne notright
        add r7, #0x2
notright:

        ldr r1, =0x101
        cmp r0, r1
        bne not1up
        ldrh r1, [r7, #0]
        sub r1, r1, #1
        strh r1, [r7, #0]
not1up:
        ldr r1, =0x120
        cmp r0, r1
        bne not1down
        ldrh r1, [r7, #0]
        add r1, r1, #1
        strh r1, [r7, #0]
not1down:

        ldr r2, =0x100
        ldr r1, =0x102
        cmp r0, r1
        bne not1left
        ldrh r1, [r7, #0]
        sub r1, r2
        strh r1, [r7, #0]
not1left:
        ldr r1, =0x110
        cmp r0, r1
        bne not1right
        ldrh r1, [r7, #0]
        add r1, r2
        strh r1, [r7, #0]
not1right:

        ldr r2, =0x100000
        ldr r1, =0x201
        cmp r0, r1
        bne not2up
        sub r7, r2
not2up: ldr r1, =0x220
        cmp r0, r1
        bne not2down
        add r7, r2
not2down:

        ldr r2, =0x1000
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
        
        ldr r1, =saoff
        str r7, [r1]

        
        ldr r0, =message1bh
        mov r1, r7
        ldr r2, =(hex8T+1)
        mov r3, #1
        mov lr, r3
        add lr, pc
        bx r2
        
        
        mov r0, #4
        mov r1, #6
        ldr r2, =message1b
        ldr r3, =0xfbfb
        ldr r4, =(lcdPutsT+1)
        mov r5, #1
        mov lr, r5
        add lr, pc
        bx r4
        
        
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
        add r0, r0, #0x10
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
        ldr r0, =(18 + (6*32))
        cmp r1, r0
        blt lfop
        
        b loopr
        
        .align
        
saoff:  .word 0x3000000
caoff:  .word 0
cyoff:  .word 156

        .ltorg
        
@ r0->data to read...
buildLine:
        push {lr}
        mov r7, r0

        ldr r0, =messagf1
        mov r1, r7
        ldr r2, =(hex2T+1)
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
        ldr r2, =(hex4T+1)
        mov r3, #1
        mov lr, r3
        add lr, pc
        bx r2
        
        add r5, #4
        add r4, #2
        sub r6, #1
        bne loopda
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

message1b:  .ascii "BASE ADDRESS: "
message1bh: .asciz "xxxxxxxx"
message1c:  .asciz "++ -00--02--04--06--08--0A--0C--0E- 0123456789ABCDEF"

message2a:  .ascii "     UP-00000010 DN+00000010 LT-00000002 RT+00000002"
            .ascii "F2+  UP-00100000 DN+00100000 LT-00001000 RT+00001000"
            .asciz "F1+  UP VAL-0001 DN VAL+0001 LT VAL-0100 RT VAL+0100"
            
        .align 4
        
intaddrs:   .word 0x00000000
            .word 0x00030000
            .word 0x00100000
            .word 0x03000000
intaddrp:   .word intaddrs

        
.include "../includes/hex.S"
.include "../includes/buttons.S"
.include "../includes/lcd.S"
.include "../includes/uart.S"
.include "../includes/color.S"

sprite:
.incbin     "image.rgb"
