@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@ hddTest
@
@ Date:     06/01/2004
@ Author:   By DoggerMoore
@
@
        .include "../includes/common.S"
        .include "../includes/av300.S"
.text

loop1:
        add r0, pc, #1
        bx r0                       @ switch to thumb
        
        .thumb
       
        ldr r0, =bufferx
        
        callThumb r0, lcdInitT

        callThumb1 r4, terminalPutsT, msg1
tryLoop:        
        callThumb r4, ataInit
        
@        mov r0, #1
@        callThumb r4, ataSetMode

        callThumb1 r4, ataIdentify, bufferx
        cmp r0, #0
         beq idok
     
        mov r1, r0
        ldr r0, =msg3e1h
        callThumb r4, hex2T
        callThumb2 r4, terminalPutsCT, msg3e1, 0xffff

        ldr r0, =0x10000
lwait1: sub r0, #1
         bne lwait1
        b tryLoop                   @ init ATA again, and try IDENTIFY again...
  
idok:
@        ldr r6, =bufferx
@        mov r5, #32        
@loopxf: 
@        mov r0, r6
@        callThumb r4, buildLine

@        ldr r0, =messagf1
@        mov r1, r6
@        ldr r2, =bufferx
@        sub r1, r2
@        callThumb r2, hex2T

@        callThumb1 r4, terminalPutsT, messagf1
@        callThumb1 r4, uartOutsT, messagf1
        
@        add r6, #16
@        sub r5, #1
@        cmp r5, #0
@         bne loopxf

@@@
         
         
        ldr r0, =(bufferx + 20)
        ldr r1, =(ID_Serial + 15)
        mov r2, #10
        bl memCopy

        ldr r0, =(bufferx + 46)
        ldr r1, =(ID_Firmware + 15)
        mov r2, #4
        bl memCopy

        ldr r0, =(bufferx + 54)
        ldr r1, =(ID_Model + 9)
        mov r2, #20
        bl memCopy
        
        ldr r0, =(bufferx + 49*2)
        ldrh r1, [r0]
        ldr r2, =0x0100
        mov r3, #'N'
        tst r1, r2
         beq nts1
        mov r3, #'Y'   
nts1:   ldr r0, =(ID_DMAS + 15)
        strb r3, [r0]
        
        ldr r2, =0x0200
        mov r3, #'N'
        tst r1, r2
         beq nts2
        mov r3, #'Y'   
nts2:   ldr r0, =(ID_LBAS + 15)
        strb r3, [r0]

        ldr r0, =(bufferx + 83*2)
        ldrh r1, [r0]
        ldr r2, =0x0400
        mov r3, #'N'
        tst r1, r2
         beq nts3
        mov r3, #'Y'   
nts3:   ldr r0, =(ID_48BitS + 15)
        strb r3, [r0]
        
        ldr r0, =(ID_TotalSecs + 15)
        ldr r2, =(bufferx + 60*2)
        ldr r1, [r2]
        callThumb r4, hex8T
        
        ldr r0, =(ID_Max48LBA + 15)
        ldr r2, =(bufferx + 102*2)
        ldr r1, [r2]
        callThumb r4, hex8T
        ldr r0, =(ID_Max48LBA + 23)
        ldr r2, =(bufferx + 100*2)
        ldr r1, [r2]
        callThumb r4, hex8T
        
        callThumb2 r4, terminalPutsCT, ID_Serial, 0x0404
        
loopi:  b loopi
@-------------------------------------------------------------------------------
         
         
         

@ r0 = source
@ r1 = dest
@ r2 = count in words
@
memCopy:push {r0, r1, r2, r3}
memCL:  ldrb r3, [r0]
        cmp r3, #0
         bne memn1
        mov r3, #'.'
memn1:
        strb r3, [r1, #1]
        ldrb r3, [r0, #1]
        cmp r3, #0
         bne memn2
        mov r3, #'.'
memn2:
        strb r3, [r1]
        add r0, #2
        add r1, #2
        sub r2, #1
        cmp r2, #0
         bne memCL
        pop {r0, r1, r2, r3}
        mov pc, lr
        
        
        
@ r0->data to read...
buildLine:
        push {lr}
        push {r4, r5, r6, r7}
        mov r7, r0

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

msg1:   .asciz "Initialising ATA...\n"
msg2:   .ascii ":"
msg2h:  .asciz "xxxx"

msg3e1: .ascii "\nATA Error! "
msg3e1h:.asciz "xx\n"

@ ------------------------------------------------------------------------------

ID_Serial:   .ascii  "Serial number [xxxxxxxxxxxxxxxxxxxx]\n"
ID_Firmware: .ascii  "Firmware rev  [xxxxxxxx]\n"
ID_Model:    .ascii  "Model   [xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx]\n"
ID_LBAS:     .ascii  "LBA Supported [x]\n"
ID_DMAS:     .ascii  "DMA Supported [x]\n"
ID_48BitS:   .ascii  "48b Supported [x]\n"
ID_TotalSecs:.ascii  "Total sectors [xxxxxxxx]\n"
ID_Max48LBA: .asciz  "Max 48LBA:    [xxxxxxxxxxxxxxxx]\n"        

@ ------------------------------------------------------------------------------

        .align
.include "../includes/hex.S"
.include "../includes/buttons.S"
.include "../includes/lcd.S"
.include "../includes/ata.S"
.include "../includes/terminal.S"
.include "../includes/uart.S"

bufferx:
        .word 0

