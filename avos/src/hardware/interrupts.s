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
@ void interruptsInit(r0->handler)

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
@ interruptsGetIRQEnabled()
.globl interruptsGetIRQEnabledA
interruptsGetIRQEnabledA:
        mrs r0, cpsr
        mvn r0, r0, lsr #7
        and r0, r0, #1
        mov pc, lr

@ ------------------------------------------------------------------------------
@ interruptsSetIRQEnabled()
.globl interruptsSetIRQEnabledA
interruptsSetIRQEnabledA:
        mrs r0, cpsr
        bic r0, r0, #0x80
        msr cpsr_c, r0
        mov pc, lr

@ ------------------------------------------------------------------------------
@ interruptsSetIRQDisabled()
.globl interruptsSetIRQDisabledA
interruptsSetIRQDisabledA:
        mrs r0, cpsr
        orr r0, r0, #0x80
        msr cpsr_c, r0
        mov pc, lr
        
@ ------------------------------------------------------------------------------
@ interruptsInit()
.globl interruptsInitA
interruptsInitA:
        switchThumb
.globl interruptsInit
.thumb_func

interruptsInit:
        ldr r2, =0x34
        str r0, [r2]            @ For the moment use original int startup code
        bx lr

@ ------------------------------------------------------------------------------
@ interruptsGetCauses()
.globl interruptsGetCausesA
interruptsGetCausesA:
        switchThumb
.globl interruptsGetCauses
.thumb_func

interruptsGetCauses:
        ldr r2, =0x30504
        ldrh r0, [r2, #2]
        lsl r0, #16
        mov r1, #0
        ldrh r1, [r2]
        orr r0, r1
        bx lr

@ ------------------------------------------------------------------------------
@ interruptsSetCauses(r0=val)
.globl interruptsSetCausesA
interruptsSetCausesA:
        switchThumb
.globl interruptsSetCauses
.thumb_func

interruptsSetCauses:
        push {r0, r2}
        ldr r2, =0x30504
        strh r0, [r2]
        lsr r0, #16
        strh r0, [r2, #2]
        pop {r0, r2}
        bx lr

@ ------------------------------------------------------------------------------
@ interruptsGetCauses()
.globl interruptsGetCauses2A
interruptsGetCauses2A:
        switchThumb
.globl interruptsGetCauses2
.thumb_func

interruptsGetCauses2:
        ldr r2, =0x30500
        ldrh r0, [r2, #2]
        lsl r0, #16
        mov r1, #0
        ldrh r1, [r2]
        orr r0, r1
        bx lr

@ ------------------------------------------------------------------------------
@ interruptsSetCauses2(r0=val)
.globl interruptsSetCauses2A
interruptsSetCauses2A:
        switchThumb
.globl interruptsSetCauses2
.thumb_func

interruptsSetCauses2:
        push {r0, r2}
        ldr r2, =0x30500
        strh r0, [r2]
        lsr r0, #16
        strh r0, [r2, #2]
        pop {r0, r2}
        bx lr
        
@ ------------------------------------------------------------------------------
@ interruptsGetMask()
.globl interruptsGetMaskA
interruptsGetMaskA:
        switchThumb
.globl interruptsGetMask
.thumb_func

interruptsGetMask:
        push {r1, r2}
        ldr r2, =0x30524
        ldrh r0, [r2, #2]
        lsl r0, #16
        mov r1, #0
        ldrh r1, [r2]
        orr r0, r1
        pop {r1, r2}
        bx lr

@ ------------------------------------------------------------------------------
@ interruptsSetMask(r0=val)
.globl interruptsSetMaskA
interruptsSetMaskA:
        switchThumb
.globl interruptsSetMask
.thumb_func

interruptsSetMask:
        push {r0, r2}
        ldr r2, =0x30524
        strh r0, [r2]
        lsr r0, #16
        strh r0, [r2, #2]
        pop {r0, r2}
        bx lr
        
@ ------------------------------------------------------------------------------
@ interruptsGetMask2()
.globl interruptsGetMask2A
interruptsGetMask2A:
        switchThumb
.globl interruptsGetMask2
.thumb_func

interruptsGetMask2:
        push {r1, r2}
        ldr r2, =0x30520
        ldrh r0, [r2, #2]
        lsl r0, #16
        mov r1, #0
        ldrh r1, [r2]
        orr r0, r1
        pop {r1, r2}
        bx lr

@ ------------------------------------------------------------------------------
@ interruptsSetMask2(r0=val)
.globl interruptsSetMask2A
interruptsSetMask2A:
        switchThumb
.globl interruptsSetMask2
.thumb_func

interruptsSetMask2:
        push {r0, r2}
        ldr r2, =0x30520
        strh r0, [r2]
        lsr r0, #16
        strh r0, [r2, #2]
        pop {r0, r2}
        bx lr

@ ------------------------------------------------------------------------------
@ interruptsResetIRQA(r0=val)
.globl interruptsResetIRQA
interruptsResetIRQA:
        switchThumb
.globl interruptsResetIRQ
.thumb_func

interruptsResetIRQ:
        cmp r0, #15
         bhi irhi
        mov r1, #1
        lsl r1, r0
        ldr r0, =0x30500
        ldrh r2, [r0, #0x20]
        tst r1, r2
         bne irl2
        ldrh r2, [r0, #4]
        orr r2, r1
        strh r2, [r0, #4]
        bx lr
        
irl2:   ldrh r2, [r0]
        orr r2, r1
        strh r2, [r0]
        bx lr
irhi:
        mov r1, #1
        lsl r1, r0
        lsr r1, #16
        ldr r0, =0x30500
        ldrh r2, [r0, #0x22]
        tst r1, r2
         bne irl3
        ldrh r2, [r0, #6]
        orr r2, r1
        strh r2, [r0, #6]
        bx lr

irl3:   ldrh r2, [r0, #2]
        orr r2, r1
        strh r2, [r0, #2]
        bx lr

@ ------------------------------------------------------------------------------
@ interruptsResetMaskA(r0=val)
.globl interruptsResetMaskA
interruptsResetMaskA:
        switchThumb
.globl interruptsResetMask
.thumb_func

interruptsResetMask:
        cmp r0, #15
         bhi irmhi
        ldr r2, =0x30524
        mov r1, #1
        lsl r1, r0
        b irmgo
irmhi:  ldr r2, =0x30526
        mov r1, #1
        lsl r1, r0
        lsr r1, #16
irmgo:  ldrh r0, [r2]
        bic r0, r1
        strh r0, [r2]
        bx lr
        
        .arm
        .ltorg

.endif
