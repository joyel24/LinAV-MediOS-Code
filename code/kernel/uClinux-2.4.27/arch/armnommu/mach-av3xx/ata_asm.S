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
ataCommand_SLEEP            =   0xe0

ataStatus_BSY               =   0x80
ataStatus_RDY               =   0x40
ataStatus_DF                =   0x20
ataStatus_DRQ               =   0x08
ataStatus_ERR               =   0x01

ataSelectLBA                =   0x40

ataSectorHalfwords          =   256
ataSectorHalfwordsShift     =   8           @ 256 halfwords

.arm        
@ ------------------------------------------------------------------------------
@ ataReadSectors(r0=LBA, r1=number, r2->buffer)
@
.globl ataReadSectors
ataReadSectors:        
        stmfd sp!, {r1-r4, lr}
        mov r3, r0
        mov r4, r1
        bl ataWaitForReady
        cmp r0, #0
        bne ataRSE
        mov r0, r3
        bl ataRead
        mov r3, #1
        movs r3, r3, lsl #ataSectorHalfwordsShift
ataRX1:
        bl ataWaitForXfer
        cmp r0, #0
        bne ataRSF
        mov r0, r2
        mov r1, r3
        bl ataReadData
        add r2, r2, r3
        add r2, r2, r3
        sub r4, r4, #1
        bne ataRX1
        mov r0, #0
        ldmfd sp!, {r1-r4,pc}
ataRSE: mov r0, #0
        sub r0, r0, #1
        ldmfd sp!, {r1-r4,pc}
ataRSF: mov r0, #0
        sub r0, r0, #2
        ldmfd sp!, {r1-r4,pc}

        
@ ------------------------------------------------------------------------------
@ ataWriteSectors(r0=LBA, r1=number, r2->buffer)
@
.globl ataWriteSectors
ataWriteSectors:
        stmfd sp!, {r1-r4, lr}
        mov r3, r0
        mov r4, r1
        bl ataWaitForReady
        cmp r0, #0
        bne ataWSE
        mov r0, r3
        bl ataWrite
        mov r3, #1
        movs r3, r3, lsl #ataSectorHalfwordsShift
ataWX1:
        bl ataWaitForXfer
        cmp r0, #0
        bne ataWSF
        mov r0, r2
        mov r1, r3
        bl ataWriteData
        add r2, r2, r3
        add r2, r2, r3
        sub r4, r4, #1
        bne ataWX1
        mov r0, #0
        ldmfd sp!, {r1-r4,pc}
ataWSE: mov r0, #0
        sub r0, r0, #1
        ldmfd sp!, {r1-r4,pc}
ataWSF: mov r0, #0
        sub r0, r0, #2
        ldmfd sp!, {r1-r4,pc}


@ ------------------------------------------------------------------------------
@ ataIdentifyDevice(r0->buffer)
@
.globl ataIdentifyDevice
ataIdentifyDevice:
        stmfd sp!, {r1-r3, lr}
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
        ldmfd sp!, {r1-r3,pc}
ataISE: mov r0, #0
        sub r0, r0, #1
        ldmfd sp!, {r1-r3,pc}
ataISF: mov r0, #0
        sub r0, r0, #2
        ldmfd sp!, {r1-r3,pc}
        
        
@ ------------------------------------------------------------------------------
@ ataRead(r0=LBA, r1=number)
@
.globl ataRead
ataRead:
        stmfd sp!, {r0,r2}
        ldr r2, =ataRegSector
        strb r0, [r2]
        ldr r2, =ataRegLCyl
        movs r0, r0, lsr #8
        strb r0, [r2]
        ldr r2, =ataRegHCyl
        movs r0, r0, lsr #8
        strb r0, [r2]
        movs r0, r0, lsr #8
        ldr r3, =ataSelectLBA        
        ldr r2, =ataRegSelect
        orr r0, r0, r3
        strb r0, [r2]        
        ldr r2, =ataRegNSector
        strb r1, [r2]
        ldr r2, =ataRegCommand
        mov r0, #ataCommand_READ_SECTORS
        strb r0, [r2]
        ldmfd sp!, {r0,r2}
        mov pc, lr

@ ------------------------------------------------------------------------------
@ ataWrite(r0=LBA, r1=number)
@
.globl ataWrite
ataWrite:
        stmfd sp!, {r0,r2}
        ldr r2, =ataRegSector
        strb r0, [r2]
        ldr r2, =ataRegLCyl
        movs r0, r0, lsr #8
        strb r0, [r2]
        ldr r2, =ataRegHCyl
        movs r0, r0, lsr #8
        strb r0, [r2]
        movs r0, r0, lsr #8
        ldr r3, =ataSelectLBA        
        ldr r2, =ataRegSelect
        orr r0, r0, r3
        strb r0, [r2]        
        ldr r2, =ataRegNSector
        strb r1, [r2]
        ldr r2, =ataRegCommand
        mov r0, #ataCommand_WRITE_SECTORS
        strb r0, [r2]
        ldmfd sp!, {r0,r2}
        mov pc, lr
        
        
@ ------------------------------------------------------------------------------
@ ataIdentify()
@
.globl ataIdentify
ataIdentify:
        stmfd sp!, {r1,r2}
        ldr r2, =ataRegSelect
        mov r1, #0
        strb r1, [r2]
        ldr r2, =ataRegCommand
        mov r1, #ataCommand_IDENTIFY
        strb r1, [r2]
        ldmfd sp!, {r1,r2}
        mov pc, lr
        


@ ------------------------------------------------------------------------------
@ ataReadData(r0->buffer, r1=#halfwords)
@
.globl ataReadData
ataReadData:
        stmfd sp!, {r1-r3}
        ldr r2, =ataRegData
ataReadD:
        ldrh r3, [r2]
        strh r3, [r0]
        add r0, r0, #2
        sub r1, r1, #1
        bne ataReadD
        ldmfd sp!, {r1-r3}
        mov pc, lr
        
        
@ ------------------------------------------------------------------------------
@ ataWriteData(r0->buffer, r1=#halfwords)
@
.globl ataWriteData
ataWriteData:
        stmfd sp!, {r1-r3}
        ldr r2, =ataRegData
ataWriteD:
        ldrh r3, [r0]
        strh r3, [r2]
        add r0, r0, #2
        sub r1, r1, #1
         bne ataWriteD
        ldmfd sp!, {r1-r3}
        mov pc, lr
        
        
@ ------------------------------------------------------------------------------
@ ataWaitForXfer()
@   Waits until xfer is ready...
@   returns -1 = Timeout
.globl ataWaitForXfer
ataWaitForXfer:
        stmfd sp!, {r1-r3}
        ldr r3, =ataWaitForXferTO
ataNotX:sub r3, r3, #1
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
        ldmfd sp!, {r1-r3}
        mov pc, lr
ataTO2: mov r0, #0
        sub r0, r0, #1
        ldmfd sp!, {r1-r3}
        mov pc, lr
        
        
@ ------------------------------------------------------------------------------
@ ataWaitForReady()
@   Waits until Ready ATA is...
@   returns -1 = Timeout
.globl ataWaitForReady
ataWaitForReady:
        stmfd sp!, {r1-r3}
        ldr r3, =ataWaitForReadyTO
ataNotR:sub r3, r3, #1
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
        ldmfd sp!, {r1-r3}
        mov pc, lr
ataTO1: mov r0, #0
        sub r0, r0, #1
        ldmfd sp!, {r1-r3}
        mov pc, lr

        
@ ------------------------------------------------------------------------------
@ ataStatus()
@   Reads the current ATA status
.globl ataStatus
ataStatus:
        stmfd sp!, {r1}
        mov r0, #0
        ldr r1, =ataRegStatus
        ldrb r0, [r1]
        ldmfd sp!, {r1}
        mov pc, lr
        
        
       
.endif
