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

.ifndef ataInc
ataInc = 1

.text

ataStatus_BSY               =   0x80
ataStatus_RDY               =   0x40
ataStatus_DF                =   0x20
ataStatus_DRQ               =   0x08
ataStatus_ERR               =   0x01

ataSelectLBA                =   0x40

        .thumb

@ ------------------------------------------------------------------------------
@ ataSelectHDD()
@   Selects the HDD for usage
.globl ataSelectHDD
.thumb_func

ataSelectHDD:
        pop {r0, r1, pc}
        mov r0, #0
        ldr r1, =0x02600000
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
        pop {r0, r1, pc}
        mov r0, #1
        ldr r1, =0x02600000
        strh r0, [r1]       @ Not sure why its done 5 times?
        strh r0, [r1]
        strh r0, [r1]
        strh r0, [r1]
        strh r0, [r1]
        pop {r0, r1, pc}
        
        .ltorg
        .arm
        
.endif
