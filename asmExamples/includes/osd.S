@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@ osd - (AV300)
@
@ Date:     15/01/2004
@ Author:   By DoggerMoore
@
@ osdSetMainConfigT (r0=byte width setting, r1=controlReg) 
@ osdSetMainOffsetT (r0->buffer)
@ osdSetMainSizeT (r0=width, r1=height)
@ osdSetMainPositionT (r0=x, r1=y)
@
@ osdSetMain2ConfigT (r0=byte width setting, r1=controlReg) 
@ osdSetMain2OffsetT (r0->buffer)
@ osdSetMain2SizeT (r0=width, r1=height)
@ osdSetMain2PositionT (r0=x, r1=y)
@
@ osdSetBackConfigT (r0=byte width setting, r1=controlReg) 
@ osdSetBackOffsetT (r0->buffer)
@ osdSetBackSizeT (r0=width, r1=height)
@ osdSetBackPositionT (r0=x, r1=y)
@
@ osdSetBack2ConfigT (r0=byte width setting, r1=controlReg) 
@ osdSetBack2OffsetT (r0->buffer)
@ osdSetBack2SizeT (r0=width, r1=height)
@ osdSetBack2PositionT (r0=x, r1=y)
@
@ osdSetCursorSizeT (r0=width, r1=height)
@ osdSetCursorPositionT (r0=x, r1=y)
@
@ osdSetCursor2PositionT (r0=x, r1=y)
@
@ osdSetScreenPositionT (r0=x, r1=y)
@ osdSetScreenConfigT (r0=config)

.include "../includes/common.S"

.ifndef osdInc
osdInc = 1

.text
        .thumb

@ osdSetScreenConfigT r0=config
@
osdSetScreenConfigT:
        push {r1}
        ldr r1, =0x30680
        strh r0, [r1]
        pop {r1}
        bx lr
        
@ osdSetScreenPositionT r0=x, r1=y
@
osdSetScreenPositionT:
        push {r2}
        ldr r2, =0x306a0
        strh r0, [r2]
        strh r1, [r2, #2]
        pop {r2}
        bx lr
        
@ osdSetMainConfigT r0=byte width, r1=config
@
osdSetMainConfigT:
        push {r0, r1, r2}
        ldr r2, =0x30684
        strh r1, [r2]
        strh r0, [r2, #0xc]
        pop {r0, r1, r2}
        bx lr

@ osdSetMain2ConfigT r0=byte width, r1=config
@
osdSetMain2ConfigT:
        push {r0, r1, r2}
        ldr r2, =0x30686
        strh r1, [r2]
        strh r0, [r2, #0xc]
        pop {r0, r1, r2}
        bx lr

@ osdSetBackConfigT r0=byte width, r1=config
@
osdSetBackConfigT:
        push {r0, r1, r2, r3}
        ldr r2, =0x30682
        mov r3, #0
        ldrh r3, [r2]
        lsr r3, #8
        lsl r3, #8
        orr r1, r3
        strh r1, [r2]
        strh r0, [r2, #0xa]
        pop {r0, r1, r2, r3}
        bx lr

@ osdSetBack2ConfigT r0=byte width, r1=config
@
osdSetBack2ConfigT:
        push {r0, r1, r2, r3}
        ldr r2, =0x30682
        mov r3, #0
        ldrh r3, [r2]
        lsl r3, #24
        lsr r3, #24
        lsl r1, #8
        orr r1, r3
        strh r1, [r2]
        strh r0, [r2, #0xc]
        pop {r0, r1, r2, r3}
        bx lr
        
@ osdSetMainOffsetT r0=offset
@
osdSetMainOffsetT:
        push {r0, r1, r2}
        ldr r1, =0x3069a
        ldr r2, =0x3000000
        sub r0, r2
        lsr r0, #5
        strh r0, [r1, #2]
        lsr r0, #16
        mov r2, #0
        ldrh r2, [r1]
        lsr r2, #8
        lsl r2, #8
        orr r2, r0
        strh r0, [r1]
        pop {r0, r1, r2}
        bx lr

@ osdSetMain2OffsetT r0=offset
@
osdSetMain2OffsetT:
        push {r0, r1, r2}
        ldr r1, =0x3069a
        ldr r2, =0x3000000
        sub r0, r2
        lsr r0, #5
        strh r0, [r1, #4]
        lsr r0, #16
        mov r2, #0
        ldrh r2, [r1]
        lsl r2, #24
        lsr r2, #24
        lsl r0, #8
        orr r2, r0
        strh r0, [r1]
        pop {r0, r1, r2}
        bx lr

@ osdSetBackOffsetT r0=offset
@
osdSetBackOffsetT:
        push {r0, r1, r2}
        ldr r1, =0x30694
        ldr r2, =0x3000000
        sub r0, r2
        lsr r0, #5
        strh r0, [r1, #2]
        lsr r0, #16
        mov r2, #0
        ldrh r2, [r1]
        lsr r2, #8
        lsl r2, #8
        orr r2, r0
        strh r0, [r1]
        pop {r0, r1, r2}
        bx lr

@ osdSetBack2OffsetT r0=offset
@
osdSetBack2OffsetT:
        push {r0, r1, r2}
        ldr r1, =0x30694
        ldr r2, =0x3000000
        sub r0, r2
        lsr r0, #5
        strh r0, [r1, #4]
        lsr r0, #16
        mov r2, #0
        ldrh r2, [r1]
        lsl r2, #24
        lsr r2, #24
        lsl r0, #8
        orr r2, r0
        strh r0, [r1]
        pop {r0, r1, r2}
        bx lr
        
@ osdSetMainSizeT r0=width, r1=height
@
osdSetMainSizeT:
        push {r2}
        ldr r2, =0x306b8
        strh r0, [r2]
        strh r1, [r2, #2]
        pop {r2}
        bx lr

@ osdSetMain2SizeT r0=width, r1=height
@
osdSetMain2SizeT:
        push {r2}
        ldr r2, =0x306c0
        strh r0, [r2]
        strh r1, [r2, #2]
        pop {r2}
        bx lr

@ osdSetBackSizeT r0=width, r1=height
@
osdSetBackSizeT:
        push {r2}
        ldr r2, =0x306a8
        strh r0, [r2]
        strh r1, [r2, #2]
        pop {r2}
        bx lr

@ osdSetBack2SizeT r0=width, r1=height
@
osdSetBack2SizeT:
        push {r2}
        ldr r2, =0x306b0
        strh r0, [r2]
        strh r1, [r2, #2]
        pop {r2}
        bx lr

@ osdSetCursorSizeT r0=width, r1=height
@
osdSetCursorSizeT:
        push {r2}
        ldr r2, =0x306c8
        strh r0, [r2]
        strh r1, [r2, #2]
        pop {r2}
        bx lr
        
@ osdSetMainPositionT r0=x, r1=y
@
osdSetMainPositionT:
        push {r2}
        ldr r2, =0x306b4
        strh r0, [r2]
        strh r1, [r2, #2]
        pop {r2}
        bx lr

@ osdSetMain2PositionT r0=x, r1=y
@
osdSetMain2PositionT:
        push {r2}
        ldr r2, =0x306bc
        strh r0, [r2]
        strh r1, [r2, #2]
        pop {r2}
        bx lr

@ osdSetBackPositionT r0=x, r1=y
@
osdSetBackPositionT:
        push {r2}
        ldr r2, =0x306a4
        strh r0, [r2]
        strh r1, [r2, #2]
        pop {r2}
        bx lr

@ osdSetBack2PositionT r0=x, r1=y
@
osdSetBack2PositionT:
        push {r2}
        ldr r2, =0x306ac
        strh r0, [r2]
        strh r1, [r2, #2]
        pop {r2}
        bx lr

@ osdSetCursorPositionT r0=x, r1=y
@
osdSetCursorPositionT:
        push {r2}
        ldr r2, =0x306c4
        strh r0, [r2]
        strh r1, [r2, #2]
        pop {r2}
        bx lr

@ osdSetCursor2PositionT r0=x, r1=y
@
osdSetCursor2PositionT:
        push {r2}
        ldr r2, =0x306cc
        strh r0, [r2]
        strh r1, [r2, #2]
        pop {r2}
        bx lr

        .ltorg
    
        .arm
        
.endif
