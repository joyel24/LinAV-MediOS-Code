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
        @.include "../includes/jbmm.S"
        .include "../includes/av300.S"
        
        doimage = 1
		@uartDebug = 1 @ send debug info on UART
		doTimeOut = 1 @ turn on the autorun of DEFAULT.BIN after time out
        
.text

        add r0, pc, #1
        bx r0                       @ switch to thumb
        .thumb

        b overb
        .space 0x46, 0
        .asciz "CJBMVERSION2000"    @ So we can update firmware to this...
        .space 0x20, 0
overb:
        .ifdef hardwareAV300
        mov r0, #0x30
        .endif
        
        .ifdef hardwareJBMM
        mov r0, #0x80
        .endif
        
        mov r2, r0
        lsl r0, #20     @ 03000000

        mov r1, #0x40
        lsl r1, #12     @ 40000

        add r2, #4
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
        
        
        callThumb r0, lcdInitT
                
        callThumb2 r5, terminalSetPosT, 0, 1
        callThumb2 r5, terminalPutsCT, msg1, 0xfbfb

        @ Wait for USB disconnect, or menu1
        callThumb r1, usbIsConnectedT
        cmp r0, #1
         bne overusbW
        callThumb2 r5, terminalPutsCT, msg1a, 0xffff        
waitUsb:
        callThumb r1, buttonsGetStatusT
        ldr r1, =0x677                      @ menu1 to skip the USB wait...
        cmp r0, r1
         beq overusbW
        callThumb r1, usbIsConnectedT
        cmp r0, #1
         bne overusbW
        b waitUsb
overusbW:

tryLoop:
        callThumb2 r5, terminalPutsCT, msg2, 0xffff
        callThumb r5, ataInitT
        callThumb2 r5, terminalPutsCT, msgDONE, 0xfbfb

        callThumb2 r4, terminalPutsCT, msg3, 0xffff
        callThumb1 r4, ataIdentifyT, bufferx
        cmp r0, #0
         beq ntryLoop

        mov r1, r0
        ldr r0, =msg3e1h
        callThumb r4, hex2T
        callThumb2 r4, terminalPutsCT, msg3e1, 0xffff

        ldr r0, =0x10000
lwait1: sub r0, #1
         bne lwait1
        b tryLoop                   @ init ATA again, and try IDENTIFY again...
        
ntryLoop:
        callThumb2 r4, terminalPutsCT, msgDONE, 0xfbfb

        ldr r0, =(bufferx + 20)
        ldr r1, =(msg4 + 12)
        mov r2, #20
        callThumb r3, memSwapT
        callThumb2 r4, terminalPutsCT, msg4, 0xfdfd

        ldr r0, =(bufferx + 46)
        ldr r1, =(msg5 + 12)
        mov r2, #8
        callThumb r3, memSwapT
        callThumb2 r4, terminalPutsCT, msg5, 0xfdfd

        ldr r0, =(bufferx + 54)
        ldr r1, =(msg6 + 12)
        mov r2, #40
        callThumb r3, memSwapT
        callThumb2 r4, terminalPutsCT, msg6, 0xfdfd


@ ------------------------------------------------------------------------------
@ Display partition info
@ ------------------------------------------------------------------------------

        callThumb2 r4, terminalPutsCT, msg100, 0xffff
        ldr r0, =bufferx
        mov r1, #1
        mov r2, #1
        callThumb r4, readLBAT
        callThumb2 r4, terminalPutsCT, msgDONE, 0xfbfb
        
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

        callThumb2 r4, terminalPutsCT, msg101, 0xfdfd

        add r5, #8
        add r6, #16
        sub r7, #1
         bne loopsp

@ ------------------------------------------------------------------------------
@ Read 1st partition
@ ------------------------------------------------------------------------------

        callThumb2 r4, terminalPutsCT, msg200, 0xffff
        ldr r0, =bufferx
        ldr r1, =part1Start
        ldr r1, [r1]
        mov r2, #1
        callThumb r4, readLBAT
        callThumb2 r4, terminalPutsCT, msgDONE, 0xfbfb

        ldr r0, =(bufferx + 3)
        ldr r1, =(msg201 + 12)
        mov r2, #8
        callThumb r4, memMoveT
        
        callThumb2 r4, terminalPutsCT, msg201, 0xfdfd

        ldr r0, =part1Start
        ldr r0, [r0]
        callThumb r2, fatInitT

        b oval1        
        .ltorg
oval1:

waitScr:
        callThumb r1, buttonsGetStatusT
        ldr r1, =0x777
        cmp r0, r1
         bne waitScr

@ ------------------------------------------------------------------------------
@ Init program selection screen
@ ------------------------------------------------------------------------------

        callThumb r0, lcdClsT      @ Clear screen...

.ifdef doimage
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

        callThumb2 r3, osdSetBackConfigT, 0x14, 0x03
        callThumb2 r3, osdSetMainConfigT, 0x14, 0xdd
.endif

        mov r0, #4
        mov r1, #6
        ldr r2, =msg_001
        ldr r3, =0xfbfb
        callThumb r4, lcdPutsT

@ ------------------------------------------------------------------------------
@ Main loop
@ ------------------------------------------------------------------------------

        ldr r0, =fatRootClu             @ Start at root dir.
        ldr r0, [r0]

        @ r0 = LBA of dir...
loopRR:
        ldr r6, =bufferx
        mov r7, r0
loopRE: mov r0, r7
        mov r1, r6
        callThumb r2, fatReadCT
        ldr r0, =fatClusterSize
        ldr r0, [r0]
        add r6, r0
        mov r0, r7
        callThumb r2, fatTraceCCT
        mov r7, r0
        ldr r0, =0x0ffffff8
        cmp r7, r0
         blt loopRE

        mov r2, #0
        ldr r7, =bufferx
lfval:  mov r0, #0
        ldrb r0, [r7, #11]
        cmp r0, #0xf
         beq noshp
        mov r1, #8
        tst r0, r1
         bne noshp
        ldrb r0, [r7]
        cmp r0, #0xe5
         beq noshp
        cmp r0, #0x00
         beq noshq              @ ERROR!
        ldr r0, =bufferx
        sub r7, r0
        ldr r0, =dirCurrentScroll
        ldr r1, =dirCurrentCursor
        str r7, [r0]
        str r7, [r1]
        b noshq
noshp:  add r7, #32
        b lfval
         
noshq:

@ ------------------------------------------------------------------------------
@ Display it on screen and process user navigation
@ ------------------------------------------------------------------------------

loopNV:
        ldr r7, =bufferx
        ldr r0, dirCurrentScroll
        add r7, r0
        mov r6, #18             @ display 18 files max at a time...
        mov r5, #36
        mov r4, #0              @ cursor position in window...
loopDV: 
        mov r0, #0
        ldrb r0, [r7, #11]
        cmp r0, #0xf
         beq nosho
        mov r1, #8
        tst r0, r1
         bne nosho
        ldrb r0, [r7]
        cmp r0, #0xe5
         beq nosho
        cmp r0, #0x00
         beq nosho2
        mov r0, r7
        ldr r1, =dirLine
        mov r2, #8
        callThumb r3, memMoveT  @ Move filename

        mov r0, r7
        add r0, #8
        ldr r1, =(dirLine + 9)
        mov r2, #3
        callThumb r3, memMoveT  @ Move ext

        push {r4}
        ldr r1, =runFile
        mov r0, r7
        mov r2, #11
        mov r3, #0
        mov r4, #0
lcmpe:  ldrb r3, [r1]
        ldrb r4, [r0]
        cmp r3, r4
         bne ncmpe
        add r1, #1
        add r0, #1
        sub r2, #1
         bne lcmpe
        
        mov r0, r7
        add r0, #20
        callThumb r1, read2T
        mov r3, r0
        lsl r3, #16
        mov r0, r7
        add r0, #26
        callThumb r1, read2T
        orr r3, r0       
        ldr r0, =runClus
        str r3, [r0]                @ save cluster # of autorun file...    
ncmpe:
        pop {r4}

        ldr r3, =0xffff
        ldr r0, =fontBG
        ldr r1, =0x0000
        str r1, [r0]
        ldr r1, =bufferx
        ldr r0, dirCurrentCursor
        add r0, r1
        cmp r0, r7
         bne notCur
        mov r4, r6
        ldr r0, =fontBG
        ldr r1, =0x0404
        str r1, [r0]
notCur:
        mov r0, #220
        mov r1, r5
        ldr r2, =dirLine
        push {r4}
        callThumb r4, lcdPutsT
        pop {r4}
        
        add r5, #6
        sub r6, #1
nosho:
        add r7, #32
        cmp r6, #0
         bne loopDV
nosho2: cmp r6, #0
         beq nobot1

        ldr r0, =fontBG
        ldr r1, =0x0000
        str r1, [r0]         
ldel2:
        mov r0, #220
        mov r1, r5
        ldr r2, =dirEmpt
        mov r3, #0
        callThumb r7, lcdPutsT
        add r5, #6
        sub r6, #1
        cmp r6, #0
         bne ldel2
nobot1:

@ ------------------------------------------------------------------------------
@ Get key input and process it
@ ------------------------------------------------------------------------------

                                @ clear out bottom entries if needed...        
                                @ r4 = cursor position so we know
                                @ if we can move up or down...
        
        callThumb r0, buttonsGetStatusT
        mov r7, r0        
        
        ldr r1, =0x0001
        tst r7, r1
         bne notup
        ldr r0, =timeoutC
        ldr r1, timeoutMax
        str r1, [r0]
        ldr r0, =dirCurrentCursor
        callThumb r2, cursorUpT
        cmp r0, #0
         beq notup
        cmp r4, #18
         bne notup
        ldr r0, =dirCurrentScroll
        callThumb r2, cursorUpT

notup:  ldr r1, =0x0020
        tst r7, r1
         bne notdn
        ldr r0, =timeoutC
        ldr r1, timeoutMax
        str r1, [r0]
        ldr r0, =dirCurrentCursor
        callThumb r2, cursorDnT
        cmp r0, #0
         beq notdn                  @ Can't go down :(
        cmp r4, #1
         bne notdn
        ldr r0, =dirCurrentScroll
        callThumb r2, cursorDnT
notdn:
        ldr r1, =0x0100
        tst r7, r1
        bne notm1
        ldr r0, dirCurrentCursor
        ldr r4, =bufferx
        add r4, r0
                                    @ r4->file entry
        
wnkd1:                                      @ Wait for no key down...
        callThumb r0, buttonsGetStatusT
        ldr r1, =0x777
        cmp r0, r1
         bne wnkd1

        mov r0, r4
        add r0, #20
        callThumb r5, read2T
        mov r3, r0
        lsl r3, #16
        mov r0, r4
        add r0, #26
        callThumb r5, read2T
        orr r0, r3
        
        mov r1, #0
        ldrb r1, [r4, #11]          @ ATTR
        mov r2, #0x10
        tst r1, r2
         beq notm1                  @ is it a dir?

        ldr r1, =dirCurrentCursor
        mov r2, #0
        str r2, [r1]
        ldr r1, =dirCurrentScroll
        str r2, [r1]
        cmp r0, #0
        bne oklp
        add r0, #2                  @ eugh! horrid fixup for root?
oklp:
        b loopRR
notm1:
        ldr r1, =0x200
        tst r7, r1
        bne notm2
        
        ldr r0, dirCurrentCursor
        ldr r4, =bufferx
        add r4, r0
                                    @ r4->file entry
        mov r0, r4
        add r0, #20
        callThumb r5, read2T
        mov r3, r0
        lsl r3, #16
        mov r0, r4
        add r0, #26
        callThumb r5, read2T
        orr r0, r3

        mov r1, #0
        ldrb r1, [r4, #11]          @ ATTR
        mov r2, #0x10
        tst r1, r2
         bne notm2                  @ is it (not a dir)?

        b launchFile                @ Launch the file...
notm2:
        ldr r0, timeoutC
        cmp r0, #0
         bne noovf
.ifndef doTimeOut
		b errt
.endif
        ldr r0, runClus
        cmp r0, #0
         beq errt
        callThumb2 r4, terminalSetPosT, 0, 35
        callThumb2 r4, terminalPutsCT, msg780, 0x0101
        ldr r0, runClus
        b launchFile                @ Lauch the default file...
errt:   ldr r0, timeoutMax

noovf:  sub r0, #1
        ldr r1, =timeoutC
        str r0, [r1]

        ldr r0, =0x10000
wait6:  sub r0, #1
         bne wait6

        b loopNV

        .ltorg

@ ------------------------------------------------------------------------------
        
partitionInfo:
part1Start:         .word   0
part1Size:          .word   0
part2Start:         .word   0
part2Size:          .word   0
part3Start:         .word   0
part3Size:          .word   0
part4Start:         .word   0
part4Size:          .word   0

timeoutC:           .word   0x100
timeoutMax:         .word   0x100
dirCurrentScroll:   .word   0
dirCurrentCursor:   .word   0

@ ------------------------------------------------------------------------------

cursorUpT:
        push {r1, r2, r3, r4, r7}
        ldr r1, [r0]
        ldr r7, =bufferx
        mov r2, #8
lcup:   cmp r1, #0
         beq errorUp            @ Can't go up any more!
        sub r1, #32
        mov r3, #0
        ldrb r3, [r7, r1]
        cmp r3, #0xe5
         beq lcup
        cmp r3, #0x0
         beq errorUp            @ ERROR!
        mov r4, r1
        add r4, #11
        ldrb r3, [r7, r4]
        cmp r3, #0x0f           @ Long filename?
         beq lcup               @ try again!
        tst r3, r2
         bne lcup
doneUp: str r1, [r0]
        mov r0, #1
        pop {r1, r2, r3, r4, r7} 
        bx lr
errorUp:mov r0, #0
        pop {r1, r2, r3, r4, r7}
        bx lr

@ ------------------------------------------------------------------------------
        
cursorDnT:
        push {r1, r2, r3, r4, r7}
        ldr r1, [r0]
        ldr r7, =bufferx
        mov r2, #8
lcdn:   
        @cmp r1, #0              @ TODO...
        @ beq errorDn            @ Can't go down any more!
        add r1, #32
        mov r3, #0
        ldrb r3, [r7, r1]
        cmp r3, #0xe5
         beq lcdn
        cmp r3, #0x0
         beq errorDn            @ ERROR!
        mov r4, r1
        add r4, #11
        ldrb r3, [r7, r4]
        cmp r3, #0x0f           @ Long filename?
         beq lcdn               @ try again!
        tst r3, r2
         bne lcdn
doneDn: str r1, [r0]
        mov r0, #1
        pop {r1, r2, r3, r4, r7} 
        bx lr
errorDn:mov r0, #0
        pop {r1, r2, r3, r4, r7} 
        bx lr

@ ------------------------------------------------------------------------------
        
launchFile:
        mov r7, r0

        callThumb2 r4, terminalSetPosT, 0, 36        
        callThumb2 r4, terminalPutsCT, msg600, 0xffff

        ldr r6, =ramStart
loopLE: 
        callThumb2 r4, terminalPutsCT, msg601, 0xffff
        mov r0, r7
        mov r1, r6
        callThumb r4, fatReadCT
        ldr r2, =fatClusterSize
        ldr r2, [r2]
        add r6, r2                      @ move to next cluster in mem...
        mov r0, r7
        callThumb r2, fatTraceCCT
        mov r7, r0
        ldr r0, =0x0ffffff8
        cmp r7, r0
         blt loopLE

        callThumb2 r4, terminalPutsCT, msg700, 0xffff

        ldr r0, =ramStart
        bx r0                           @ EXECUTE IT!

@ ------------------------------------------------------------------------------

readLBAT:
        push {r1, r2, r3, r4, r5, r6, lr}
        mov r4, r0
        mov r5, r1
        mov r6, r2
readTry:
        mov r0, r4
        mov r1, r5
        mov r2, r6
        callThumb r3, ataReadLBAT
        cmp r0, #0
         beq nreadtry

        mov r1, r0
        ldr r0, =msg3e1h
        callThumb r3, hex2T
        callThumb2 r3, terminalPutsCT, msg3e1, 0xffff

        ldr r0, =0x10000
readw1: sub r0, #1
         bne readw1
        b readTry                   @ try again...
nreadtry:
        pop {r1, r2, r3, r4, r5, r6}
        pop {r0}
        bx r0

@ ------------------------------------------------------------------------------

        .ltorg

runClus:    .word   0
runFile:    .ascii  "DEFAULT BIN"

        .align 4

@ ------------------------------------------------------------------------------

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

@ ------------------------------------------------------------------------------

memSwapT:
        push {r0, r1, r2, r3, r4}
lopS:	ldrb r3, [r0]
		add r0, #1
		ldrb r4, [r0]
        strb r4, [r1]
		add r1, #1
		strb r3, [r1]
        add r0, #1
        add r1, #1
        sub r2, #2
        bne lopS
        pop {r0, r1, r2, r3, r4}
        bx lr

@ ------------------------------------------------------------------------------

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

@ ------------------------------------------------------------------------------

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

@ ------------------------------------------------------------------------------

        .ltorg
        
msg1:   .asciz " -- avOS BootLoader version 0.4a (c)2004 DoGgEr -- \n\n"
msg1a:  .asciz "USB Cable attached. Waiting for disconnect...\n"
msg2:   .asciz "Initialising ATA...                        "
msg3:   .asciz "Identifying hard drive...                  "

msg3e1: .ascii "\nATA Error! "
msg3e1h:.asciz "xx\n"

msg4:   .asciz " Serial:    xxxxxxxxxxxxxxxxxxxx\n"
msg5:   .asciz " Firmware:  xxxxxxxx            \n"
msg6:   .asciz " Model:     xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n"
msg100: .asciz "Reading MBR...                             "
msg101: .asciz " Partition: xx xx xxxxxxxx xxxxxxxx\n"
msg200: .asciz "Reading BootSector (Partition 1)...        "
msg201: .asciz " OEM:       xxxxxxxx\n"
msg600: .asciz "Loading image...                           \n"
msg601: .asciz "."
msg700: .asciz "\nRunning image...                           \n"
msg780: .asciz "Timed out!                                 \n"
msgDONE:  .asciz "[ DONE ]\n"

msg_001:.asciz " -- avOs BootLoader Version 0.4a (c)2004 DoGgEr -- "

dirLine:.asciz "xxxxxxxx.xxx"
dirEmpt:.asciz ".           "

        .align 4
.include "../includes/hex.S"
.include "../includes/buttons.S"
.include "../includes/lcd.S"
.include "../includes/ata.S"
.include "../includes/fat.S"
.include "../includes/terminal.S"
.include "../includes/uart.S"
.include "../includes/usb.S" 
.include "../includes/color.S"

        .align 4
fatbuffLBA: .word 0
fatbuff:    .space 512, 0
bufferx:    .space 512, 0

.ifdef doimage
sprite:
.incbin     "image.rgb"
.endif
