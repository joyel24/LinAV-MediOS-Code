@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@ FAT
@
@ Date:     17/01/2004
@ Author:   By DoggerMoore
@
@ fatInitT      r0=LBA of this partition
@ fatReadCT     r0=Cluster, r1->buffer
@ fatTraceCCT   r0=cluster
@               returns r0=next cluster
@

.ifndef fatInc
fatInc = 1

.text

        fatSectorSize       =   512
        fatSectorSizeShift  =   9

        .thumb

@ fatInitT (r0=LBA of Partition)
@
fatInitT:
        push {lr}
        push {r1, r2, r3, r4, r5, r6, r7}
        mov r7, r0
        ldr r0, =fatTBuffer
        mov r1, r7
        mov r2, #1
        callThumb r4, ataReadLBAT
        
        ldr r6, =fatTBuffer
        mov r0, r6
        add r0, #36             @ BPB_FATSz32
        callThumb r3, fatRead4T
        mov r5, r0

        mov r0, #0
        ldrb r0, [r6, #16]      @ BPB_NumFATs
        mul r5, r0

        mov r0, r6
        add r0, #14             @ BPB_RsvdSecCnt
        callThumb r3, fatRead2T
        add r0, r7
        ldr r1, =fatLBAFAT
        str r0, [r1]
        add r5, r0
        ldr r4, =fatLBADATA
        str r5, [r4]
        
        mov r0, r6
        add r0, #44             @ BPB_RootClus
        callThumb r3, fatRead4T
        ldr r1, =fatRootClu
        str r0, [r1]       
        
        mov r1, #0
        ldrb r1, [r6, #13]
        ldr r0, =fatSecPerClu        
        str r1, [r0]
        
        lsl r1, #9              @ * 512
        ldr r0, =fatClusterSize
        str r1, [r0]

        pop {r1, r2, r3, r4, r5, r6, r7}
        pop {r0}
        bx r0
        
@ fatReadCT  r0=cluster, r1->buffer
@
fatReadCT:
        push {lr}
        push {r1, r2, r3, r4, r5, r6, r7}
        mov r7, r0
        mov r6, r1

        @ Read the data up...
        ldr r5, fatSecPerClu
        mov r4, r7
        sub r4, #2
        mul r4, r5
        ldr r0, fatLBADATA
        add r4, r0
fatReadM:
        mov r0, r6
        mov r1, r4
        mov r2, #1
        callThumb r3, ataReadLBAT
        ldr r0, =fatSectorSize
        add r6, r0
        add r4, #1                                          @ Move to next LBA
        sub r5, #1
         bne fatReadM
        pop {r1, r2, r3, r4, r5, r6, r7}
        pop {r0}
        bx r0

@ fatTraceCCT r0=cluster
@            returns next cluster in chain.
fatTraceCCT:
        push {lr}
        push {r1, r2, r3, r4, r5, r6, r7}
        mov r7, r0
        lsl r7, #2                                         @ *4
        mov r5, r7
        lsr r5, #fatSectorSizeShift
        ldr r1, fatLBAFAT
        add r1, r5

        ldr r0, =fatFATBufferLBA
        ldr r5, [r0]
        cmp r1, r5
         beq fatCacheHit
        str r1, [r0]

        ldr r0, =fatFATBuffer
        mov r2, #1
        callThumb r3, ataReadLBAT

fatCacheHit:        
        ldr r5, =(fatSectorSize - 1)
        and r5, r7                                         @ = offset
        ldr r4, =fatFATBuffer
        ldr r7, [r4, r5]

        mov r0, r7        
        pop {r1, r2, r3, r4, r5, r6, r7}
        pop {r1}
        bx r1
        
fatRead4T:
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

fatRead2T:
        push {r1, r2}
        mov r1, #0
        ldrb r1, [r0, #0x01]
        lsl r1, #8
        ldrb r2, [r0, #0x00]
        orr r1, r2
        mov r0, r1
        pop {r1, r2}
        bx lr
        
        .ltorg
        
fatClusterSize:     .word   0
fatSecPerClu:       .word   0
fatRootClu:         .word   0

fatLBAFAT:          .word   0
fatLBADATA:         .word   0

fatTBuffer:         .space  fatSectorSize, 0
fatFATBuffer:       .space  fatSectorSize, 0
fatFATBufferLBA:    .word   0

        .arm
        
.endif
