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

.text

ataStatus_BSY   =       0x80
ataStatus_RDY   =       0x40
ataStatus_DF    =       0x20
ataStatus_DRQ   =       0x08
ataStatus_ERR   =       0x01

ataRegNSector   =       0x02400080
ataRegSector    =       0x02400100
ataRegLCyl      =       0x02400180
ataRegHCyl      =       0x02400200
ataRegSelect    =       0x02400280
ataRegCommand   =       0x02400380
ataRegStatus    =       0x02400380

ataRegData      =       0x02400000


        .thumb
        
@ ataWaitForStartOfXferT
@
ataWaitForStartOfXferT:
        push {lr}
        ldr r1, =(ataWaitForBSYT+1)
        mov r2, #1
        mov lr, r2
        add lr, pc
        bx r1
        
        @ TODO: CHECK ALT_STATUS
        
        pop {r0}
        bx r0
        
@ ataWaitForRDYT
@
ataWaitForRDYT:
        push {r1, r2, lr}
notred: ldr r1, =(ataGetStatusT+1)
        mov r2, #1
        mov lr, r2
        add lr, pc
        bx r1
        ldr r2, =ataStatus_RDY
        tst r0, r2
        beq notred
        pop {r1, r2}
        pop {r0}
        bx r0

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
        ldr r1, =0x02400380
        mov r0, #0
        ldrh r0, [r1]
        pop {r1}
        bx lr
        
@ ataInitT
@
ataInitT:
        push {r0, r1, r2, r3, r4}
        @ do hardware writes...
        
        ldr r0, =hardwrites
loophw: ldr r1, [r0]
        cmp r1, #0
        beq donehw
        mov r2, #0
        mov r3, #0
        ldrh r2, [r0, #4]
        ldrh r3, [r0, #6]
        
        ldrh r4, [r1]
        and r4, r2
        orr r4, r3
        strh r4, [r1]
        
        
        add r0, #8
        b loophw
donehw:
        pop {r0, r1, r2, r3, r4}
        bx lr

        .ltorg
hardwrites:
        .word 0x00030522, 0x00000800
        .word 0x00030502, 0x00000800
        .word 0x00030526, 0x00000800
        .word 0x00030404, 0x000003e7
        .word 0x00030406, 0x0000001a
        .word 0x00030400, 0x00000001
        .word 0x00030402, 0x00000001
        .word 0x00030524, 0x00000000
        .word 0x00030526, 0x00000800
        .word 0x00030500, 0x0000ffff
        .word 0x00030502, 0x0000ffff
        .word 0x00030504, 0x0000ffff
        .word 0x00030506, 0x0000ffff
        .word 0x00030590, 0x00000000
        .word 0x00030590, 0x00000050
        .word 0x00030592, 0x00000000
        .word 0x00030594, 0x00000000
        .word 0x0003088e, 0x00000001
        .word 0x00030880, 0x00000021
        .word 0x00030888, 0x00000000
        .word 0x00030888, 0x00000000
        .word 0x0003088e, 0x00000000
        .word 0x0003088e, 0x00000004
@        .word 0x00030884, 0x00000031       @ makes it crash...
        .word 0x00030888, 0x00000000
        .word 0x00030888, 0x00000000
        .word 0x0003088e, 0x00000000
        .word 0x02600300, 0x00000008        @ SPIN UP HDD
        .word 0x00030206, 0x000000ff
        .word 0x00030286, 0x000000ff   
        .word 0x00030204, 0x00000000   
        .word 0x00030204, 0x00000000   
        .word 0x00030700, 0x00000001   
        .word 0x00030702, 0x00000000   
        .word 0x0003071c, 0x00000000   
        .word 0x0003071e, 0x00000000   
        .word 0x00030780, 0x00000000   
        .word 0x00030782, 0x00000000   
        .word 0x0003078c, 0x00000008   
        .word 0x0003078e, 0x00000280   
        .word 0x00030790, 0x00000008   
        .word 0x00030792, 0x00000200   
        .word 0x00030794, 0x00000081   
        .word 0x00030796, 0x00000000   
        .word 0x00030798, 0x000000d0   
        .word 0x0003079a, 0x00000000   
        .word 0x0003079c, 0x00002000   
        .word 0x0003079e, 0x00000020   
        .word 0x000307a0, 0x00000000   
        .word 0x000307a2, 0x00002000   
        .word 0x000307a4, 0x00000020   
        .word 0x000307a6, 0x00000000   
        .word 0x000307a8, 0x00000000   
        .word 0x000307aa, 0x00000000   
        .word 0x000307ac, 0x00000000   
        .word 0x000307ae, 0x00000000   
        .word 0x000307b0, 0x00000000   
        .word 0x000307b2, 0x00000000   
        .word 0x000307b4, 0x00000000   
        .word 0x000307b6, 0x00000000   
        .word 0x000307b8, 0x00000000   
        .word 0x000307ba, 0x00000000   
        .word 0x000307bc, 0x00000010   
        .word 0x000307be, 0x00000010  
        .word 0x000307c0, 0x00000000   
        .word 0x000307c2, 0x00000000   
        .word 0x000307c4, 0x00004000   
        .word 0x000307c6, 0x00000000   
        .word 0x000307c8, 0x00004000   
        .word 0x000307ca, 0x00000000   
        .word 0x000307cc, 0x00004000   
        .word 0x000307ce, 0x00000000   
        .word 0x000307d0, 0x00000000   
        .word 0x000307d2, 0x00000000   
        .word 0x000307d4, 0x00000008   
        .word 0x000307d6, 0x0000e743   
        .word 0x000307d8, 0x00000d37   
        .word 0x000307da, 0x00008d3c   
        .word 0x000307dc, 0x00004730   
        .word 0x000307de, 0x00008a00   
        .word 0x000307e0, 0x00000000   
        .word 0x000307e2, 0x00000000   
        .word 0x000307e4, 0x00001100   
        .word 0x000307e6, 0x00000000   
        .word 0x000307ea, 0x0000eb10   
        .word 0x000307ec, 0x0000eb10   


        .word 0x00000000, 0x00000000   

        
        .arm
