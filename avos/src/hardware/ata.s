@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@ ATA
@
@ Date:     14/02/2004
@ Author:   By DoggerMoore
@
@ void ataSelectHDD()
@ void ataSelectMemoryCard()
@ void ataPowerUpHDD()
@ void ataPowerDownHDD()
@ u32 ataStatus()
@ u32 ataWaitForReady()
@ u32 ataWaitForXfer()
@ void ataWriteData(r0->buffer, r1=#halfwords)
@ void ataReadData(r0->buffer, r1=#halfwords)
@
@ void ataIdentify()
@ void ataRead(r0=LBA, r1=number)
@ void ataWrite(r0=LBA, r1=number)
@

.ifndef ataInc
ataInc = 1

.text

ataWaitForReadyTO           =   0x200000
ataWaitForXferTO            =   0x200000

ataRegSelectSource          =   0x02600000          @ JBMM = 0x02400000
ataRegPowerUpHDD            =   0x02600300          @ JBMM = 0x02400200
ataValPowerUpHDD            =   0x0a                @ JBMM = 0x04

ataRegData                  =   0x02400000          @ JBMM = 0x04000800
ataRegError                 =   0x02400080          @ JBMM = 0x04000900
ataRegNSector               =   0x02400100          @ JBMM = 0x04000a00
ataRegSector                =   0x02400180          @ JBMM = 0x04000b00
ataRegLCyl                  =   0x02400200          @ JBMM = 0x04000c00
ataRegHCyl                  =   0x02400280          @ JBMM = 0x04000d00
ataRegSelect                =   0x02400300          @ JBMM = 0x04000e00
ataRegStatus                =   0x02400340          @ JBMM = 0x04000f00
ataRegCommand               =   0x02400380          @ JBMM = 0x04000f00

ataCommand_READ_SECTORS     =   0x20
ataCommand_WRITE_SECTORS    =   0x30
ataCommand_IDENTIFY         =   0xec

ataStatus_BSY               =   0x80
ataStatus_RDY               =   0x40
ataStatus_DF                =   0x20
ataStatus_DRQ               =   0x08
ataStatus_ERR               =   0x01

ataSelectLBA                =   0x40

        .thumb

@ ------------------------------------------------------------------------------
@ ataRead(r0=LBA, r1=number)
@
.globl ataRead
.thumb_func

ataRead:
        push {r0, r2, lr}
        ldr r2, =ataRegSector
        strb r0, [r2]
        ldr r2, =ataRegLCyl
        lsr r0, #8
        strb r0, [r2]
        ldr r2, =ataRegHCyl
        lsr r0, #8
        strb r0, [r2]
        lsr r0, #8
        ldr r3, =ataSelectLBA        
        ldr r2, =ataRegSelect
        orr r0, r3
        strb r0, [r2]        
        ldr r2, =ataRegNSector
        strb r1, [r2]
        ldr r2, =ataRegCommand
        mov r0, #ataCommand_READ_SECTORS
        strb r0, [r2]
        pop {r0, r2, pc}

@ ------------------------------------------------------------------------------
@ ataWrite(r0=LBA, r1=number)
@
.globl ataWrite
.thumb_func

ataWrite:
        push {r0, r2, lr}
        ldr r2, =ataRegSector
        strb r0, [r2]
        ldr r2, =ataRegLCyl
        lsr r0, #8
        strb r0, [r2]
        ldr r2, =ataRegHCyl
        lsr r0, #8
        strb r0, [r2]
        lsr r0, #8
        ldr r3, =ataSelectLBA        
        ldr r2, =ataRegSelect
        orr r0, r3
        strb r0, [r2]        
        ldr r2, =ataRegNSector
        strb r1, [r2]
        ldr r2, =ataRegCommand
        mov r0, #ataCommand_WRITE_SECTORS
        strb r0, [r2]
        pop {r0, r2, pc}
        
        
@ ------------------------------------------------------------------------------
@ ataIdentify()
@
.globl ataIdentify
.thumb_func

ataIdentify:
        push {r1, r2, lr}
        ldr r2, =ataRegSelect
        mov r1, #0
        strb r1, [r2]
        ldr r2, =ataRegCommand
        mov r1, #ataCommand_IDENTIFY
        strb r1, [r2]
        pop {r1, r2, pc}
        
        
@ ------------------------------------------------------------------------------
@ ataReadData(r0->buffer, r1=#halfwords)
@
.globl ataReadData
.thumb_func

ataReadData:
        push {r1, r2, r3, lr}
        ldr r2, =ataRegData
ataReadD:
        ldrh r3, [r2]
        strh r3, [r0]
        add r0, #2
        sub r1, #1
         bne ataReadD
        pop {r1, r2, r3, pc}

        
@ ------------------------------------------------------------------------------
@ ataWriteData(r0->buffer, r1=#halfwords)
@
.globl ataWriteData
.thumb_func

ataWriteData:
        push {r1, r2, r3, lr}
        ldr r2, =ataRegData
ataWriteD:
        ldrh r3, [r0]
        strh r3, [r2]
        add r0, #2
        sub r1, #1
         bne ataWriteD
        pop {r1, r2, r3, pc}
        
        
@ ------------------------------------------------------------------------------
@ ataWaitForXfer()
@   Waits until xfer is ready...
@   returns -1 = Timeout
.globl ataWaitForXfer
.thumb_func

ataWaitForXfer:
        push {r1, r2, r3, lr}
        ldr r3, =ataWaitForXferTO
ataNotX:sub r3, #1
         beq ataTO2
        mov r0, #0
        ldr r1, =ataRegStatus
        ldrb r0, [r1]
        mov r2, #ataStatus_BSY
        tst r0, r2
         bne ataNotX                @ BSY Set! Can't do much now
        mov r2, #ataStatus_DRQ
        tst r0, r2
         beq ataNotX                @ DRQ Clear!
        mov r0, #0
        pop {r1, r2, r3, pc}
ataTO2: mov r0, #0
        sub r0, #1
        pop {r1, r2, r3, pc}
        
        
@ ------------------------------------------------------------------------------
@ ataWaitForReady()
@   Waits until Ready ATA is...
@   returns -1 = Timeout
.globl ataWaitForReady
.thumb_func

ataWaitForReady:
        push {r1, r2, r3, lr}
        ldr r3, =ataWaitForReadyTO
ataNotR:sub r3, #1
         beq ataTO1
        mov r0, #0
        ldr r1, =ataRegStatus
        ldrb r0, [r1]
        mov r2, #ataStatus_BSY
        tst r0, r2
         bne ataNotR                @ BSY Set! Can't do much now
        mov r2, #ataStatus_RDY
        tst r0, r2
         beq ataNotR                @ RDY Clear!
        mov r0, #0
        pop {r1, r2, r3, pc}
ataTO1: mov r0, #0
        sub r0, #1
        pop {r1, r2, r3, pc}

        
@ ------------------------------------------------------------------------------
@ ataStatus()
@   Reads the current ATA status
.globl ataStatus
.thumb_func

ataStatus:
        push {r1, lr}
        mov r0, #0
        ldr r1, =ataRegStatus
        ldrb r0, [r1]
        pop {r1, pc}
        
        
@ ------------------------------------------------------------------------------
@ ataPowerUpHDD()
@   Supplys power to the HDD
.globl ataPowerUpHDD
.thumb_func

ataPowerUpHDD:
        push {r0, r1, lr}
        mov r0, #ataValPowerUpHDD
        ldr r1, =ataRegPowerUpHDD
        strh r0, [r1]
        pop {r0, r1, pc}


@ ------------------------------------------------------------------------------
@ ataPowerDownHDD()
@   Supplys no power to the HDD
.globl ataPowerDownHDD
.thumb_func

ataPowerDownHDD:
        push {r0, r1, lr}
        mov r0, #0
        ldr r1, =ataRegPowerUpHDD
        strh r0, [r1]
        pop {r0, r1, pc}
        
        
@ ------------------------------------------------------------------------------
@ ataSelectHDD()
@   Selects the HDD for usage
.globl ataSelectHDD
.thumb_func

ataSelectHDD:
        push {r0, r1, lr}
        mov r0, #0
        ldr r1, =ataRegSelectSource
        strh r0, [r1]       @ Not sure why its done 5 times?
        strh r0, [r1]
        strh r0, [r1]
        strh r0, [r1]
        strh r0, [r1]
        pop {r0, r1, pc}


@ ------------------------------------------------------------------------------
@ ataSelectMemoryCard()
@   Selects the MemoryCard for usage
.globl ataSelectMemoryCard
.thumb_func

ataSelectMemoryCard:
        push {r0, r1, lr}
        mov r0, #1
        ldr r1, =ataRegSelectSource
        strh r0, [r1]       @ Not sure why its done 5 times?
        strh r0, [r1]
        strh r0, [r1]
        strh r0, [r1]
        strh r0, [r1]
        pop {r0, r1, pc}
        
        .ltorg
        .arm
        
.endif
