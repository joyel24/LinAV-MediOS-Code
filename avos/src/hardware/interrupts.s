@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@ interrupts
@
@ Date:     04/01/2004
@ Author:   By DoggerMoore
@
@ u32 interruptsGetCauses()
@ void interruptsClearCauses(r0=val)
@ void interruptsSetMask(r0=mask)
@

.macro switchThumb
        .arm
        add ip, pc, #1
        bx ip
        .thumb
.endm

.ifndef interruptsInc
interruptsInc = 1

.text

        
@ ------------------------------------------------------------------------------
@ interruptsGetCauses()
.globl interruptsGetCausesA
interruptsGetCausesA:
        switchThumb
.globl interruptsGetCauses
.thumb_func

interruptsGetCauses:
        push {r1, r2}
        ldr r2, =0x30502
        ldrh r0, [r2, #2]
        lsl r0, #16
        mov r1, #0
        ldrh r0, [r2]
        orr r0, r1
        pop {r1, r2}
        bx lr

@ ------------------------------------------------------------------------------
@ interruptsClearCauses(r0=bits to clear)
.globl interruptsClearCausesA
interruptsClearCausesA:
        switchThumb
.globl interruptsClearCauses
.thumb_func

interruptsClearCauses:
        push {r0, r2}
        ldr r2, =0x30502
        strh r0, [r2]
        lsr r0, #16
        strh r0, [r2, #2]
        pop {r0, r2}
        bx lr

@ ------------------------------------------------------------------------------
@ interruptsSetMask(r0=bits to enable)
.globl interruptsSetMaskA
interruptsSetMaskA:
        switchThumb
.globl interruptsSetMask
.thumb_func

interruptsSetMask:
        push {r0, r2}
        ldr r2, =0x30522
        strh r0, [r2]
        lsr r0, #16
        strh r0, [r2, #2]
        pop {r0, r2}
        bx lr
        
        
        .arm
        .ltorg

.endif
