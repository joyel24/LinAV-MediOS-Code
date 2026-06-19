@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@ video
@
@ Date:     29/03/2004
@ Author:   By DoggerMoore
@
@ void videoSetBrightness (r0=0-255)
@ void videoSetContrast (r0=0-255)
@

.macro switchThumb
        .arm
        add ip, pc, #1
        bx ip
        .thumb
.endm
        
.ifndef videoInc
videoInc = 1

.text
        .thumb

@ ------------------------------------------------------------------------------
@ videoSetLCDOn
@
.globl videoLCDEnableA
videoLCDEnableA:
        switchThumb
.globl videoLCDEnable
.thumb_func

videoLCDEnable:
        ldr r1, =0x30804
        ldrh r2, [r1]
        mov r3, #1
        lsl r3, #15
        orr r2, r3
        strh r2, [r1]
        bx lr

@ ------------------------------------------------------------------------------
@ osdLCDDisable
@
.globl videoLCDDisableA
videoLCDDisableA:
        switchThumb
.globl osdLCDDisable
.thumb_func

videoLCDDisable:
        ldr r1, =0x30804
        ldrh r2, [r1]
        ldr r3, =0x7fff
        and r2, r3
        strh r2, [r1]
        bx lr
        

@ ------------------------------------------------------------------------------
@ videoSetBrightness r0=(0-255)
@
.globl videoSetBrightnessA
videoSetBrightnessA:
        switchThumb
.globl videoSetBrightness
.thumb_func

videoSetBrightness:
        push {r0, r1, r2}
        ldr r1, =0x30830
        ldrh r2, [r1]
        lsr r2, #8
        lsl r2, #8
        orr r2, r0
        strh r2, [r1]
        pop {r0, r1, r2}
        bx lr


@ ------------------------------------------------------------------------------
@ videoSetContrast r0=(0-255)
@
.globl videoSetContrastA
videoSetContrastA:
        switchThumb
.globl videoSetContrast
.thumb_func

videoSetContrast:
        push {r0, r1, r2}
        ldr r1, =0x30830
        ldrh r2, [r1]
        lsl r2, #24
        lsr r2, #24
        lsl r0, #8
        orr r2, r0
        strh r2, [r1]
        pop {r0, r1, r2}
        bx lr
 
@ ------------------------------------------------------------------------------
@ videoSetMode
@
.globl videoSetModeA
videoSetModeA:
        switchThumb
.globl videoSetMode
.thumb_func

videoSetMode:
        ldr r1, =0x30800
        strh r0, [r1]
        bx lr
        
        .ltorg
        .arm
        
.endif
