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
@ osdSetCursorConfigT (r0=controlReg)
@ osdSetCursorSizeT (r0=width, r1=height)
@ osdSetCursorPositionT (r0=x, r1=y)
@
@ osdSetCursor2PositionT (r0=x, r1=y)
@
@ osdSetScreenPositionT (r0=x, r1=y)
@ osdSetScreenConfigT (r0=config)
@
@ osdSetPalletteColorT (r0=Y,r1=Cr,r2=Cb, r3=index)
@
@ ostInitT

@.include "../includes/common.S"

.ifndef osdInc
osdInc = 1

.text
        .thumb

@ osdSetPalletteColorT
@
.globl osdSetPalletteColorT
.thumb_func
osdSetPalletteColorT:
        push {r0, r1, r2, r3, r4, r5}
        mov r5, #0
        ldr r4, =0x306f4
osdWait:ldrh r5, [r4]
        cmp r5, #0
         bne osdWait
        lsl r1, #8
        orr r3, r1
        strh r3, [r4, #4]
osdWaiu:ldrh r5, [r4]
        cmp r5, #0
         bne osdWaiu
        lsl r0, #8
        orr r0, r2
        strh r0, [r4, #2]
        pop {r0, r1, r2, r3, r4, r5}
        bx lr
        
@ osdSetScreenConfigT r0=config
@
.globl osdSetScreenConfigT
.thumb_func
osdSetScreenConfigT:
        push {r1}
        ldr r1, =0x30680
        strh r0, [r1]
        pop {r1}
        bx lr
        
@ osdSetScreenPositionT r0=x, r1=y
@
.globl osdSetScreenPositionT
.thumb_func
osdSetScreenPositionT:
        push {r2}
        ldr r2, =0x306a0
        strh r0, [r2]
        strh r1, [r2, #2]
        pop {r2}
        bx lr

@ osdSetCursorConfigT r0=config
@
.globl osdSetCursorConfigT
.thumb_func
osdSetCursorConfigT:
        push {r2}
        ldr r2, =0x30688
        strh r0, [r2]
        pop {r2}
        bx lr
        
@ osdSetMainConfigT r0=byte width, r1=config
@
.globl osdSetMainConfigT
.thumb_func
osdSetMainConfigT:
        push {r0, r1, r2}
        ldr r2, =0x30684
        strh r1, [r2]
        strh r0, [r2, #0xc]
        pop {r0, r1, r2}
        bx lr

@ osdSetMain2ConfigT r0=byte width, r1=config
@
.globl osdSetMain2ConfigT
.thumb_func
osdSetMain2ConfigT:
        push {r0, r1, r2}
        ldr r2, =0x30686
        strh r1, [r2]
        strh r0, [r2, #0xc]
        pop {r0, r1, r2}
        bx lr

@ osdSetBackConfigT r0=byte width, r1=config
@
.globl osdSetBackConfigT
.thumb_func
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
.globl osdSetBack2ConfigT
.thumb_func
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
.globl osdSetMainOffsetT
.thumb_func
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
        strh r2, [r1]
        pop {r0, r1, r2}
        bx lr

@ osdSetMain2OffsetT r0=offset
@
.globl osdSetMain2OffsetT
.thumb_func
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
        strh r2, [r1]
        pop {r0, r1, r2}
        bx lr

@ osdSetBackOffsetT r0=offset
@
.globl osdSetBackOffsetT
.thumb_func
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
        strh r2, [r1]
        pop {r0, r1, r2}
        bx lr

@ osdSetBack2OffsetT r0=offset
@
.globl osdSetBack2OffsetT
.thumb_func
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
        strh r2, [r1]
        pop {r0, r1, r2}
        bx lr
        
@ osdSetMainSizeT r0=width, r1=height
@
.globl osdSetMainSizeT
.thumb_func
osdSetMainSizeT:
        push {r2}
        ldr r2, =0x306b8
        strh r0, [r2]
        strh r1, [r2, #2]
        pop {r2}
        bx lr

@ osdSetMain2SizeT r0=width, r1=height
@
.globl osdSetMain2SizeT
.thumb_func
osdSetMain2SizeT:
        push {r2}
        ldr r2, =0x306c0
        strh r0, [r2]
        strh r1, [r2, #2]
        pop {r2}
        bx lr

@ osdSetBackSizeT r0=width, r1=height
@
.globl osdSetBackSizeT
.thumb_func
osdSetBackSizeT:
        push {r2}
        ldr r2, =0x306a8
        strh r0, [r2]
        strh r1, [r2, #2]
        pop {r2}
        bx lr

@ osdSetBack2SizeT r0=width, r1=height
@
.globl osdSetBack2SizeT
.thumb_func
osdSetBack2SizeT:
        push {r2}
        ldr r2, =0x306b0
        strh r0, [r2]
        strh r1, [r2, #2]
        pop {r2}
        bx lr

@ osdSetCursorSizeT r0=width, r1=height
@
.globl osdSetCursorSizeT
.thumb_func
osdSetCursorSizeT:
        push {r2}
        ldr r2, =0x306c8
        strh r0, [r2]
        strh r1, [r2, #2]
        pop {r2}
        bx lr
        
@ osdSetMainPositionT r0=x, r1=y
@
.globl osdSetMainPositionT
.thumb_func
osdSetMainPositionT:
        push {r2}
        ldr r2, =0x306b4
        strh r0, [r2]
        strh r1, [r2, #2]
        pop {r2}
        bx lr

@ osdSetMain2PositionT r0=x, r1=y
@
.globl osdSetMain2PositionT
.thumb_func
osdSetMain2PositionT:
        push {r2}
        ldr r2, =0x306bc
        strh r0, [r2]
        strh r1, [r2, #2]
        pop {r2}
        bx lr

@ osdSetBackPositionT r0=x, r1=y
@
.globl osdSetBackPositionT
.thumb_func
osdSetBackPositionT:
        push {r2}
        ldr r2, =0x306a4
        strh r0, [r2]
        strh r1, [r2, #2]
        pop {r2}
        bx lr

@ osdSetBack2PositionT r0=x, r1=y
@
.globl osdSetBack2PositionT
.thumb_func
osdSetBack2PositionT:
        push {r2}
        ldr r2, =0x306ac
        strh r0, [r2]
        strh r1, [r2, #2]
        pop {r2}
        bx lr

@ osdSetCursorPositionT r0=x, r1=y
@
.globl osdSetCursorPositionT
.thumb_func
osdSetCursorPositionT:
        push {r2}
        ldr r2, =0x306c4
        strh r0, [r2]
        strh r1, [r2, #2]
        pop {r2}
        bx lr

@ osdSetCursor2PositionT r0=x, r1=y
@
.globl osdSetCursor2PositionT
.thumb_func
osdSetCursor2PositionT:
        push {r2}
        ldr r2, =0x306cc
        strh r0, [r2]
        strh r1, [r2, #2]
        pop {r2}
        bx lr

@ osdInitT
@
.globl osdInitT
.thumb_func
osdInitT:
        push {r0, r1, r2}
    .ifdef hardwareAV300
        mov r2, #0
        ldr r0, =0x30800
        ldr r1, =0x4071
        strh r1, [r0]
        ldr r1, =0x0f40
        strh r1, [r0, #0x2]
        ldr r1, =0x8100
        strh r1, [r0, #0x4]
        ldr r1, =0x4c0f
        strh r1, [r0, #0x6]
        ldr r1, =0xaaaa
        strh r1, [r0, #0x8]
        strh r2, [r0, #0xa]
        strh r2, [r0, #0xc]
        strh r2, [r0, #0xe]
        strh r2, [r0, #0x10]
        strh r2, [r0, #0x12]
        strh r2, [r0, #0x14]
        strh r2, [r0, #0x16]
        strh r2, [r0, #0x18]
        ldr r1, =0x02e0
        strh r1, [r0, #0x1a]
        ldr r1, =0x0085             @ X offset for display
        strh r1, [r0, #0x1c]
        ldr r1, =0x0280             @ WIDTH in pixels (Number processed)
        strh r1, [r0, #0x1e]
        ldr r1, =0x0103
        strh r1, [r0, #0x20]
        ldr r1, =0x0013             @ Y offset for display
        strh r1, [r0, #0x22]
        ldr r1, =0x00f0             @ HEIGHT in pixels (number processed)
        strh r1, [r0, #0x24]
        strh r2, [r0, #0x26]
        strh r2, [r0, #0x28]
        strh r2, [r0, #0x2a]
        ldr r1, =0x8001
        strh r1, [r0, #0x2c]
        ldr r1, =0x0002
        strh r1, [r0, #0x2e]
        ldr r1, =0xff00             @ Brightness adder (lo)
        strh r1, [r0, #0x30]
        strh r2, [r0, #0x32]
        strh r2, [r0, #0x34]
        strh r2, [r0, #0x36]
        ldr r1, =0x0022
        strh r1, [r0, #0x38]
    .endif
        
    .ifdef hardwareJBMM
        ldr r1, =0x30900
        ldr r0, =0x6102
        strh r0, [r1]
        ldr r1, =0x30902
        ldr r0, =0x0400
        strh r0, [r1]
        ldr r1, =0x30680
        mov r0, #1
        strh r0, [r1]
        mov r0, #2
        strh r0, [r1, #2]
        ldr r0, =0x3f0
        strh r0, [r1, #4]
        ldr r0, =lcdVideoBuffer
        ldr r1, =0x30692
        ldr r2, =0x8000000
        sub r0, r2
        lsr r0, #5
        strh r0, [r1, #2]
        lsr r0, #16
        strh r0, [r1]
        ldr r0, =0x8f
        strh r0, [r1, #4]
        ldr r0, =0x15
        strh r0, [r1, #6]
        ldr r0, =0x294
        strh r0, [r1, #0x08]
        ldr r0, =0xea
        strh r0, [r1, #0x0a]
        ldr r0, =0x00
        strh r0, [r1, #0x1c]
        ldr r0, =0x00
        strh r0, [r1, #0x1e]
        ldr r0, =0x294
        strh r0, [r1, #0x20]
        ldr r0, =0xea
        strh r0, [r1, #0x22]
        ldr r0, =0x69
        strh r0, [r1, #0x28]
        ldr r0, =0x30612
        ldr r1, =0x14
        strh r1, [r0]
    .endif
        
        ldr r0, =lcdRegBacklight
        ldr r1, =0x4
        strh r1, [r0]

    .ifdef hardwareAV300        
        ldr r0, =0x30804
        ldr r1, =0x8100
        strh r1, [r0]
        
        ldr r0, =lcdRegBacklight
        ldr r1, =0x4
        strh r1, [r0]
    .endif        
        pop {r0, r1, r2}
        bx lr
        
        .ltorg
    
        .arm
        
.endif
