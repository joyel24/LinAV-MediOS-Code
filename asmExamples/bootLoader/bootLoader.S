@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@ bootLoader
@
@ Date:     17/01/2004
@ Author:   By DoggerMoore
@
@
        .include "../includes/common.S"

.text

        add r0, pc, #1
        bx r0                       @ switch to thumb

        .thumb

        mov r0, #0x30
        lsl r0, #20     @ 03000000

        mov r1, #0x40
        lsl r1, #8      @ 4000

        mov r2, #0x34
        lsl r2, #20     @ 03400000

lrel:   ldr r3, [r0]
        str r3, [r2]
        add r0, #4
        add r2, #4
        sub r1, #1
        bne lrel

        mov r2, #0x4
        lsl r2, #20     @ 00400000
        add r2, #1
        add r2, pc
        bx r2           @ *** *** JUMP UP TO HIGH RAM
        

        ldr r0, =bufferx
        
        callThumb r0, lcdInitT
        callThumb2 r4, terminalPutsCT, msg1, 0x0404

        callThumb2 r4, terminalPutsCT, msg2, 0xffff
        callThumb r4, ataInitT
        callThumb2 r4, terminalPutsCT, msgDONE, 0x0202

        ldr r0, =0x100000
waity:  sub r0, #1
        bne waity
        
        callThumb2 r4, terminalPutsCT, msg3, 0xffff
        callThumb1 r4, ataIdentifyT, bufferx
        callThumb2 r4, terminalPutsCT, msgDONE, 0x0202

        ldr r0, =(bufferx + 20)
        ldr r1, =(msg4 + 12)
        mov r2, #20
        callThumb r3, memMoveT
        callThumb2 r4, terminalPutsCT, msg4, 0xfbfb
        
        ldr r0, =(bufferx + 46)
        ldr r1, =(msg5 + 12)
        mov r2, #8
        callThumb r3, memMoveT
        callThumb2 r4, terminalPutsCT, msg5, 0xfbfb

        ldr r0, =(bufferx + 54)
        ldr r1, =(msg6 + 12)
        mov r2, #40
        callThumb r3, memMoveT
        callThumb2 r4, terminalPutsCT, msg6, 0xfbfb

        callThumb2 r4, terminalPutsCT, msg100, 0xffff
        ldr r0, =bufferx
        mov r1, #1
        mov r2, #1
        callThumb r4, ataReadLBAT
        callThumb2 r4, terminalPutsCT, msgDONE, 0x0202

        ldr r6, =(bufferx+510-0x40)
        mov r7, #4
        ldr r5, =partitionInfo
loopsp:
        ldrb r1, [r6]
        ldr r0, =(msg101 + 12)
        callThumb r2, hex2T
        
        ldrb r1, [r6, #4]
        ldr r0, =(msg101 + 15)
        callThumb r2, hex2T
        
        mov r0, r6
        add r0, #8
        callThumb r2, read4T
        mov r1, r0
        str r1, [r5]
        ldr r0, =(msg101 + 18)
        callThumb r2, hex8T

        mov r0, r6
        add r0, #0x0c
        callThumb r2, read4T
        mov r1, r0
        str r1, [r5, #4]
        ldr r0, =(msg101 + 27)
        callThumb r2, hex8T
        
        callThumb2 r4, terminalPutsCT, msg101, 0xfbfb

        add r5, #8
        add r6, #16
        sub r7, #1
        bne loopsp

        callThumb2 r4, terminalPutsCT, msg200, 0xffff
        ldr r0, =bufferx
        ldr r1, part1Start
        mov r2, #1
        callThumb r4, ataReadLBAT
        callThumb2 r4, terminalPutsCT, msgDONE, 0x0202
        
        ldr r0, =(bufferx + 3)
        ldr r1, =(msg201 + 12)
        mov r2, #8
        callThumb r4, memMoveT
        
        callThumb2 r4, terminalPutsCT, msg201, 0xfbfb

        ldr r0, part1Start
        callThumb r2, fatInitT

        @ldr r0, =fatRootClu
        @sub r0, #2
        mov r0, #2
        mov r7, r0
loopRE: callThumb2 r4, terminalPutsCT, msg300, 0xffff

        mov r0, r7
        ldr r1, =bufferx
        callThumb r2, fatReadCT
        mov r0, r7
        callThumb r2, fatTraceCCT
        mov r7, r0
        callThumb r0, processDirT
        ldr r0, =0x0ffffff8
        cmp r7, r0
        blt loopRE

        ldr r0, =runClus
        ldr r0, [r0]
        cmp r0, #0
        beq alldo

        mov r7, r0
        callThumb2 r4, terminalPutsCT, msg600, 0xffff

        ldr r6, =0x03000000
loopLE: 
        mov r0, r7
        mov r1, r6
        callThumb r2, fatReadCT
        ldr r1, =fatClusterSize
        ldr r1, [r1]
        add r6, r1                      @ move to next cluster in mem...
        mov r0, r7
        callThumb r2, fatTraceCCT
        mov r7, r0
        ldr r0, =0x0ffffff8
        cmp r7, r0
        blt loopLE

        callThumb2 r4, terminalPutsCT, msg680, 0xffff
        
lwaitk: callThumb r4, buttonsGetStatusT
        ldr r1, =0x100
        tst r0, r1
         bne lwaitk
        
        callThumb2 r4, terminalPutsCT, msg700, 0xffff

        ldr r0, =0x03000000
        bx r0                           @ EXECUTE IT!
        
alldo:
        callThumb2 r4, terminalPutsCT, msg900, 0xffff        
loopr:
        b loopr

        .ltorg

runClus:    .word   0
runFile:    .ascii  "IMAGE   AVS"
        
        .align 4

partitionInfo:
part1Start: .word   0
part1Size:  .word   0
part2Start: .word   0
part2Size:  .word   0
part3Start: .word   0
part3Size:  .word   0
part4Start: .word   0
part4Size:  .word   0

        
processDirT:
        push {lr}
        push {r4, r5, r6, r7}
        ldr r7, =bufferx
        mov r6, #0
loppd:
        mov r0, r7
        add r0, r6
        ldrb r1, [r0, #11]
        
        cmp r1, #0x0f                                      @ long filename?
        beq dshow

        ldr r0, =(msg400 + 32)
        callThumb r4, hex2T
        
        mov r0, r7
        add r0, r6
        mov r1, #0
        ldrb r1, [r0]
        cmp r1, #0xe5
        beq dshow                                          @ Do not show this
        cmp r1, #0
        beq dquit                                          @ Do not show +quit
        
        ldr r1, =(msg400 + 1)
        mov r2, #11
        callThumb r4, memMoveT

        mov r0, r7
        add r0, r6
        add r0, #28
        callThumb r4, read4T
        mov r1, r0
        ldr r0, =(msg400 + 18)
        callThumb r4, hex8T

        mov r0, r7
        add r0, r6
        add r0, #20
        callThumb r4, read2T
        mov r5, r0
        lsl r5, #16
        mov r0, r7
        add r0, r6
        add r0, #26
        callThumb r4, read2T
        orr r5, r0

        mov r1, r5
        ldr r0, =(msg400 + 41)
        callThumb r4, hex8T

        callThumb2 r4, terminalPutsCT, msg400, 0xfbfb
        
        ldr r1, =runFile
        mov r0, r7
        add r0, r6
        mov r2, #11
        mov r3, #0
        mov r4, #0
lcmpf:  ldrb r3, [r1]
        ldrb r4, [r0]
        cmp r3, r4
        bne ncmpf
        add r1, #1
        add r0, #1
        sub r2, #1
        bne lcmpf

        ldr r0, =runClus
        str r5, [r0]        
ncmpf:

dshow:
        add r6, #32
        ldr r4, =fatClusterSize
        ldr r4, [r4]
        cmp r6, r4
        blt loppd
dquit:  pop {r4, r5, r6, r7}
        pop {r1}
        bx r1
        
        
memMoveT:
        push {r0, r1, r2, r3}
lopMove:ldrb r3, [r0]
        strb r3, [r1]
        add r0, #1
        add r1, #1
        sub r2, #1
        bne lopMove
        pop {r0, r1, r2, r3}
        bx lr

read4T:
        push {r1, r2}
        ldrb r1, [r0, #0x03]
        lsl r1, #8
        ldrb r2, [r0, #0x02]
        orr r1, r2
        lsl r1, #8
        ldrb r2, [r0, #0x01]
        orr r1, r2
        lsl r1, #8
        ldrb r2, [r0, #0x00]
        orr r1, r2
        mov r0, r1
        pop {r1, r2}
        bx lr

read2T:
        push {r1, r2}
        mov r1, #0
        ldrb r1, [r0, #0x01]
        lsl r1, #8
        ldrb r2, [r0, #0x00]
        orr r1, r2
        mov r0, r1
        pop {r1, r2}
        bx lr
                
msg1:   .asciz " -- avOS BootLoader version 0.02a (c)2004 DoGgEr -- \n\n"
msg2:   .asciz "Initialising ATA...                        "
msg3:   .asciz "Identifying hard drive...                  "
msg4:   .asciz " Serial:    xxxxxxxxxxxxxxxxxxxx\n"
msg5:   .asciz " Firmware:  xxxxxxxx            \n"
msg6:   .asciz " Model:     xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n"
msg100: .asciz "Reading MBR...                             "
msg101: .asciz " Partition: xx xx xxxxxxxx xxxxxxxx\n"
msg200: .asciz "Reading BootSector (Partition 1)...        "
msg201: .asciz " OEM:       xxxxxxxx\n"
msg300: .asciz "Loading root directory entries...          \n"
msg400: .asciz " xxxxxxxxxxx SIZE=xxxxxxxx ATTR=xx CLUST=xxxxxxxx\n"
msg500: .asciz " FAT ENTRY: xxxxxxxx\n"
msg600: .asciz "Loading image...                           \n"
msg680: .asciz "Press menu1 key to run image...            \n"
msg700: .asciz "Running image...                           \n"
msg900: .asciz "ALL DONE\n"
msgDONE:  .asciz "[ DONE ]\n"

        .ltorg


        .align 4
.include "../includes/hex.S"
.include "../includes/buttons.S"
.include "../includes/lcd.S"
.include "../includes/ata.S"
.include "../includes/fat.S"
.include "../includes/terminal.S"
.include "../includes/uart.S"


        .align 4
fatbuffLBA: .word 0
fatbuff:    .space 512, 0
bufferx:    .space 512, 0

