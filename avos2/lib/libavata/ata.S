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
@ void ataSelect(r0=val)
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
@ u32 ataReadSectors(r0=LBA, r1=number, r2->buffer)
@ u32 ataWriteSectors(r0=LBA, r1=number, r2->buffer)
@ u32 ataIndentifyDevice(r0->buffer)
@

.macro switchThumb
        .arm
        add ip, pc, #1
        bx ip
        .thumb
.endm

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

ataSectorHalfwords          =   256
ataSectorHalfwordsShift     =   8           @ 256 halfwords

        .thumb
        
@ ------------------------------------------------------------------------------
@ ataReadSectors(r0=LBA, r1=number, r2->buffer)
@
.globl ataReadSectorsA
ataReadSectorsA:
        switchThumb
.globl ataReadSectors
.thumb_func

ataReadSectors:
        push {r1, r2, r3, r4, lr}
        mov r3, r0
        mov r4, r1
        bl ataWaitForReady
        cmp r0, #0
         bne ataRSE
        mov r0, r3
        bl ataRead
        mov r3, #1
        lsl r3, #ataSectorHalfwordsShift
ataRX1:
        bl ataWaitForXfer
        cmp r0, #0
         bne ataRSF
        mov r0, r2
        mov r1, r3
        bl ataReadData
        add r2, r3
        add r2, r3
        sub r4, #1
         bne ataRX1
        mov r0, #0
        pop {r1, r2, r3, r4}
        pop {r1}
        bx r1
ataRSE: mov r0, #0
        sub r0, #1
        pop {r1, r2, r3, r4}
        pop {r1}
        bx r1
ataRSF: mov r0, #0
        sub r0, #2
        pop {r1, r2, r3, r4}
        pop {r1}
        bx r1

        
@ ------------------------------------------------------------------------------
@ ataWriteSectors(r0=LBA, r1=number, r2->buffer)
@
.globl ataWriteSectorsA
ataWriteSectorsA:
        switchThumb
.globl ataWriteSectors
.thumb_func

ataWriteSectors:
        push {r1, r2, r3, r4, lr}
        mov r3, r0
        mov r4, r1
        bl ataWaitForReady
        cmp r0, #0
         bne ataWSE
        mov r0, r3
        bl ataWrite
        mov r3, #1
        lsl r3, #ataSectorHalfwordsShift
ataWX1:
        bl ataWaitForXfer
        cmp r0, #0
         bne ataWSF
        mov r0, r2
        mov r1, r3
        bl ataWriteData
        add r2, r3
        add r2, r3
        sub r4, #1
         bne ataWX1
        mov r0, #0
        pop {r1, r2, r3, r4}
        pop {r1}
        bx r1
ataWSE: mov r0, #0
        sub r0, #1
        pop {r1, r2, r3, r4}
        pop {r1}
        bx r1
ataWSF: mov r0, #0
        sub r0, #2
        pop {r1, r2, r3, r4}
        pop {r1}
        bx r1


@ ------------------------------------------------------------------------------
@ ataIdentifyDevice(r0->buffer)
@
.globl ataIdentifyDeviceA
ataIdentifyDeviceA:
        switchThumb
.globl ataIdentifyDevice
.thumb_func

ataIdentifyDevice:
        push {r1, r2, r3, lr}
        mov r3, r0
        bl ataWaitForReady
        cmp r0, #0
         bne ataISE
        bl ataIdentify
        bl ataWaitForXfer
        cmp r0, #0
         bne ataISF
        mov r0, r3
        ldr r1, =ataSectorHalfwords
        bl ataReadData
        mov r0, #0
        pop {r1, r2, r3}
        pop {r1}
        bx r1
ataISE: mov r0, #0
        sub r0, #1
        pop {r1, r2, r3}
        pop {r1}
        bx r1
ataISF: mov r0, #0
        sub r0, #2
        pop {r1, r2, r3}
        pop {r1}
        bx r1
        
        
@ ------------------------------------------------------------------------------
@ ataRead(r0=LBA, r1=number)
@
.globl ataReadA
ataReadA:
        switchThumb
.globl ataRead
.thumb_func

ataRead:
        push {r0, r2}
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
        pop {r0, r2}
        bx lr

@ ------------------------------------------------------------------------------
@ ataWrite(r0=LBA, r1=number)
@
.globl ataWriteA
ataWriteA:
        switchThumb
.globl ataWrite
.thumb_func

ataWrite:
        push {r0, r2}
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
        pop {r0, r2}
        bx lr
        
        
@ ------------------------------------------------------------------------------
@ ataIdentify()
@
.globl ataIdentifyA
ataIdentifyA:
        switchThumb
.globl ataIdentify
.thumb_func

ataIdentify:
        push {r1, r2}
        ldr r2, =ataRegSelect
        mov r1, #0
        strb r1, [r2]
        ldr r2, =ataRegCommand
        mov r1, #ataCommand_IDENTIFY
        strb r1, [r2]
        pop {r1, r2}
        bx lr
        
        
@ ------------------------------------------------------------------------------
@ ataReadData(r0->buffer, r1=#halfwords)
@
.globl ataReadDataA
ataReadDataA:
        switchThumb
.globl ataReadData
.thumb_func

ataReadData:
        push {r1, r2, r3}
        ldr r2, =ataRegData
ataReadD:
        ldrh r3, [r2]
        strh r3, [r0]
        add r0, #2
        sub r1, #1
         bne ataReadD
        pop {r1, r2, r3}
        bx lr
        
        
@ ------------------------------------------------------------------------------
@ ataWriteData(r0->buffer, r1=#halfwords)
@
.globl ataWriteDataA
ataWriteDataA:
        switchThumb
.globl ataWriteData
.thumb_func

ataWriteData:
        push {r1, r2, r3}
        ldr r2, =ataRegData
ataWriteD:
        ldrh r3, [r0]
        strh r3, [r2]
        add r0, #2
        sub r1, #1
         bne ataWriteD
        pop {r1, r2, r3}
        bx lr
        
        
@ ------------------------------------------------------------------------------
@ ataWaitForXfer()
@   Waits until xfer is ready...
@   returns -1 = Timeout
.globl ataWaitForXferA
ataWaitForXferA:
        switchThumb
.globl ataWaitForXfer
.thumb_func

ataWaitForXfer:
        push {r1, r2, r3}
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
        pop {r1, r2, r3}
        bx lr
ataTO2: mov r0, #0
        sub r0, #1
        pop {r1, r2, r3}
        bx lr
        
        
@ ------------------------------------------------------------------------------
@ ataWaitForReady()
@   Waits until Ready ATA is...
@   returns -1 = Timeout
.globl ataWaitForReadyA
ataWaitForReadyA:
        switchThumb
.globl ataWaitForReady
.thumb_func

ataWaitForReady:
        push {r1, r2, r3}
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
        pop {r1, r2, r3}
        bx lr
ataTO1: mov r0, #0
        sub r0, #1
        pop {r1, r2, r3}
        bx lr

        
@ ------------------------------------------------------------------------------
@ ataStatus()
@   Reads the current ATA status
.globl ataStatusA
ataStatusA:
        switchThumb
.globl ataStatus
.thumb_func

ataStatus:
        push {r1}
        mov r0, #0
        ldr r1, =ataRegStatus
        ldrb r0, [r1]
        pop {r1}
        bx lr
        
        
@ ------------------------------------------------------------------------------
@ ataPowerUpHDD()
@   Supplys power to the HDD
.globl ataPowerUpHDDA
ataPowerUpHDDA:
        switchThumb
.globl ataPowerUpHDD
.thumb_func

ataPowerUpHDD:
        push {r0, r1}
        mov r0, #ataValPowerUpHDD
        ldr r1, =ataRegPowerUpHDD
        strh r0, [r1]
        pop {r0, r1}
        bx lr


@ ------------------------------------------------------------------------------
@ ataPowerDownHDD()
@   Supplys no power to the HDD
.globl ataPowerDownHDDA
ataPowerDownHDDA:
        switchThumb
.globl ataPowerDownHDD
.thumb_func

ataPowerDownHDD:
        push {r0, r1}
        mov r0, #0
        ldr r1, =ataRegPowerUpHDD
        strh r0, [r1]
        pop {r0, r1}
        bx lr
        
        
@ ------------------------------------------------------------------------------
@ ataSelectHDD()
@   Selects the HDD for usage
.globl ataSelectHDDA
ataSelectHDDA:
        switchThumb
.globl ataSelectHDD
.thumb_func

ataSelectHDD:
        push {lr}
        mov r0, #0
        bl ataSelect
        pop {r1}
        bx r1

@ ------------------------------------------------------------------------------
@ ataSelect()
@   Selects 
.globl ataSelectA
ataSelectA:
        switchThumb
.globl ataSelect
.thumb_func

ataSelect:
        push {r1}
        ldr r1, =ataRegSelectSource
        strh r0, [r1]       @ Not sure why its done 5 times?
        strh r0, [r1]
        strh r0, [r1]
        strh r0, [r1]
        strh r0, [r1]
        pop {r1}
        bx lr

@ ------------------------------------------------------------------------------
@ ataSelectMemoryCard()
@   Selects the MemoryCard for usage
.globl ataSelectMemoryCardA
ataSelectMemoryCardA:
        switchThumb
.globl ataSelectMemoryCard
.thumb_func

ataSelectMemoryCard:
        push {lr}
        mov r0, #1
        bl ataSelect
        pop {r1}        
        bx r1
        
        .ltorg
        .arm
        
.endif
