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
@ Date:     09/01/2004
@ Author:   By DoggerMoore
@
@ ataInitT()
@ ataGetStatusT()           returns ATA status in r0
@ ataWaitForRDYT()          Waits until ATA is ready...
@ ataWaitForBSYT()          Waits until ATA is busy...
@ ataWaitForStartOfXferT    Wait until start of xfer.
@
@ ataIdentifyT  r0->buffer
@ ataReadLBAT   r0->buffer r1=LBA, r2=n
@

.ifndef ataInc
ataInc = 1

.text

ataCommand_READ_SECTORS     =   0x20
ataCommand_READ_MULTIPLE    =   0xc4
ataCommand_SET_MULTIPLE_MODE=   0xc6
ataCommand_IDENTIFY         =   0xec

ataStatus_BSY               =   0x80
ataStatus_RDY               =   0x40
ataStatus_DF                =   0x20
ataStatus_DRQ               =   0x08
ataStatus_ERR               =   0x01

ataSelectLBA                =   0x40

        .thumb

@ ataReadLBAT (r0->buffer, r1=lba, r2=n)
@
ataReadLBAT:
        push {lr}
        push {r1, r2, r3, r4, r5, r6}
        mov r5, r0
        mov r4, r1
        mov r6, r2

        ldr r0, =0x02600300
        mov r1, #8
        strh r1, [r0]                        @ Spin up the HDD Please...        

        callThumb r1, ataWaitForRDYT        

        ldr r2, =ataRegSector
        mov r1, r4
        strb r1, [r2]
        ldr r2, =ataRegLCyl
        lsr r1, #8
        strb r1, [r2]
        ldr r2, =ataRegHCyl
        lsr r1, #8
        strb r1, [r2]

        lsr r1, #8
        ldr r3, =ataSelectLBA        
        ldr r2, =ataRegSelect
        orr r1, r3
        strb r1, [r2]

        ldr r2, =ataRegNSector
        strb r6, [r2]
        
        ldr r2, =ataRegCommand
        ldr r1, =ataCommand_READ_SECTORS
        strb r1, [r2]

        callThumb r4, ataWaitForStartOfXferT

        lsl r6, #8              @ * 256
        
        ldr r2, =ataRegData
        mov r1, #0
ataR2:  ldrh r1, [r2]
        strh r1, [r5]
        add r5, #2
        sub r6, #1
         bne ataR2

        pop {r1, r2, r3, r4, r5, r6}        
        pop {r0}
        bx r0

@ ataIdentifyT (r0->buffer)
@
ataIdentifyT:
        push {lr}
        push {r1, r2, r3, r4, r5}
        mov r5, r0

        ldr r0, =0x02600300
        mov r1, #8
        strh r1, [r0]                        @ Spin up the HDD Please...        

        callThumb r1, ataWaitForRDYT
        mov r1, r0
        mov r0, #1
        cmp r1, #0
         bne ataRT
        
        ldr r2, =ataRegSelect
        ldr r1, =0
        strb r1, [r2]
        
        ldr r2, =ataRegCommand
        ldr r1, =ataCommand_IDENTIFY
        strb r1, [r2]

        callThumb r4, ataWaitForStartOfXferT
        mov r1, r0
        mov r0, #2
        cmp r1, #0
         bne ataRT

        ldr r3, =256
        ldr r2, =ataRegData
ataRl:  mov r1, #0
        ldrh r1, [r2]
        strh r1, [r5]
        add r5, #2
        sub r3, #1
        bne ataRl
      
        mov r0, #0          @ ok...
ataRT:
        pop {r1, r2, r3, r4, r5}        
        pop {r1}
        bx r1
        
@ ataWaitForStartOfXferT
@
ataWaitForStartOfXferT:
        push {lr}
        push {r2, r3}

        ldr r3, =0x100000
notrea: mov r0, #1
        sub r3, #1
         beq tout2
        callThumb r1, ataGetStatusT
        ldr r2, =(ataStatus_DRQ)
        and r0, r2
        cmp r0, r2
         bne notrea
        mov r0, #0
tout2:
        @ TODO: CHECK ALT_STATUS
        pop {r2, r3}
        pop {r1}
        bx r1
        
@ ataWaitForRDYT
@
ataWaitForRDYT:
        push {lr}
        push {r2, r3}
        ldr r3, =0x100000
notred: mov r0, #1
        sub r3, #1
         beq tout1
        callThumb r1, ataGetStatusT
        ldr r2, =ataStatus_RDY
        tst r0, r2
        beq notred
        mov r0, #0  
tout1:  pop {r2, r3}
        pop {r1}
        bx r1

@ ataWaitForBSYT
@
ataWaitForBSYT:
        push {r1, r2, lr}
notreb: ldr r1, =(ataGetStatusT+1)
        mov r2, #1
        mov lr, r2
        add lr, pc
        bx r1
        ldr r2, =ataStatus_BSY
        tst r0, r2
        beq notreb
        pop {r1, r2}
        pop {r0}
        bx r0
        
@ ataGetStatusT
@
ataGetStatusT:
        push {r1}
        ldr r1, =ataRegStatus
        mov r0, #0
        ldrh r0, [r1]
        pop {r1}
        bx lr
        
@ ataInitT
@
ataInitT:
        push {lr}
        push {r0, r1}

        ldr r0, =0x02600000
        mov r1, #0
        strh r1, [r0]
        strh r1, [r0]
        strh r1, [r0]
        strh r1, [r0]
        strh r1, [r0]

        ldr r0, =0x02600100
        mov r1, #1
        strh r1, [r0]        
        
        ldr r0, =0x02600300
        mov r1, #0x0a
        strh r1, [r0]                        @ Spin up the HDD Please...        

        ldr r0, =0x02600100
        mov r1, #0
        strh r1, [r0]                        @ ?        
        
        pop {r0, r1}
        pop {r0}
        bx r0

@ ------------------------------------------------------------------------------
        .ltorg
        .arm
        
.endif
