@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@ Buttons - Get button status
@
@ Date:     04/01/2004
@ Author:   By DoggerMoore
@
@ u32 buttonsGetStatus()
@

.macro switchThumb
        .arm
        add ip, pc, #1
        bx ip
        .thumb
.endm

.ifndef buttonsInc
buttonsInc = 1

.text

        BUTTONS_AV300_UP        =   0x0001
        BUTTONS_AV300_LEFT      =   0x0002
        BUTTONS_AV300_RIGHT     =   0x0010
        BUTTONS_AV300_DOWN      =   0x0020
        BUTTONS_AV300_MENU3     =   0x0040
        BUTTONS_AV300_MENU1     =   0x0100
        BUTTONS_AV300_MENU2     =   0x0200
        BUTTONS_AV300_JOYPRESS  =   0x0400
        BUTTONS_AV300_ON        =   0x1000
        BUTTONS_AV300_OFF       =   0x2000
        BUTTONS_AV300_ANY       =   0x3777
        
        BUTTONS_JBMM_UP         =   0x0080
        BUTTONS_JBMM_DOWN       =   0x0008
        BUTTONS_JBMM_LEFT       =   0x0001
        BUTTONS_JBMM_RIGHT      =   0x0004
        BUTTONS_JBMM_MENU1      =   0x0010
        BUTTONS_JBMM_MENU2      =   0x0020
        BUTTONS_JBMM_MENU3      =   0x0040
        BUTTONS_JBMM_PLAY       =   0x0002
        BUTTONS_JBMM_ANY        =   0x00ff
        
@ TODO: JBMM/AV100
        
@ ------------------------------------------------------------------------------
@ buttonsGetStatus()
@   Returns button status in r0
.globl buttonsGetStatusA
buttonsGetStatusA:
        switchThumb
.globl buttonsGetStatus
.thumb_func

buttonsGetStatus:
        push {r1, r2, r3}
        mov r3, #0x0f
        ldr r2, =0x2600680
        ldrh r1, [r2, #0]
        and r1, r3
        mov r0, r1

        ldr r2, =0x2600700
        ldrh r1, [r2, #0]
        and r1, r3
        lsl r1, r1, #4
        orr r0, r1
        
        ldr r2, =0x2600780
        ldrh r1, [r2, #0]
        and r1, r3
        lsl r1, r1, #8
        orr r0, r1

        ldr r2, =0x30588
        ldrh r1, [r2]
        lsl r1, #31
        lsr r1, #19
        orr r0, r1

        ldrh r1, [r2, #2]
        lsl r1, #1
        ldr r2, =0x2000
        and r1, r2
        orr r0, r1
        
        ldr r2, =0x3fff
        eor r0, r2

        pop {r1, r2, r3}
        bx lr
        
        .arm
        .ltorg

.endif
