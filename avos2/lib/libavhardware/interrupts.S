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
@ interruptsGetFIQEnabled()
.globl interruptsGetFIQEnabledA
interruptsGetFIQEnabledA:
        mrs r0, cpsr
        mvn r0, r0, lsr #6
        and r0, r0, #1
        mov pc, lr

@ ------------------------------------------------------------------------------
@ interruptsSetFIQEnabled()
.globl interruptsSetFIQEnabledA
interruptsSetFIQEnabledA:
        mrs r0, cpsr
        bic r0, r0, #0x40
        msr cpsr_c, r0
        mov pc, lr
        
@ ------------------------------------------------------------------------------
@ interruptsSetFIQDisabled()
.globl interruptsSetFIQDisabledA
interruptsSetFIQDisabledA:
        mrs r0, cpsr
        orr r0, r0, #0x40
        msr cpsr_c, r0
        mov pc, lr

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
@ interruptsInitFIQ()
.globl interruptsInitFIQA
interruptsInitFIQA:
        switchThumb
.globl interruptsInitFIQ
.thumb_func

interruptsInitFIQ:
        ldr r2, =0x38
        ldr r0, =fiqHandle
        str r0, [r2]            @ For the moment use original int startup code
        bx lr
        
        .arm
fiqHandle:
	ldr     r8, =0x30502
	ldrh    r9, [r8]
	tst     r9, #0x800
	 bne     avFIQout
	ldrh    r8, [r8,#0x24]
	tst     r8, #0x800
	 beq     avFIQout		@ wrong FIQ or MASK => out
	
    ldr     r8, =0x3058A
	ldrh    r9, [r8]
	movs    r9, r9, lsr #0xD
	 bcs     iniAvLoopVal2
	ldr     r8, =.avLOOPval2
	ldr     r8, [r9]
	add     r8, r8, #1
	str     r8, [r9]
	ldr     r9, =0x138800
	cmp     r8, r9
	 ble		normalFIQ
	mov     r9, #1
	mov     r9, r9, lsl r8
	ldr     r8,=0x30526    @ interrupt mask 1
	ldrh    r10, [r8]
	bic     r10, r10, r9
	strh    r10, [r8]
	@mov		r8,#0
	@ldr		r9,=0x30a1a
	@strh	r8,[r9]
	b       endFiq
iniAvLoopVal2:
	mov     R8, #0
	ldr     R9, =.avLOOPval2
	str     r8, [r9]
normalFIQ:
	ldr		r8, =.avLOOPval1
	ldr		r9,[r8]
	add		r9,r9,#1
	str		r9,[r8]
	mov		r8,#1
	add		r8,r8,r9
	ldr     r9, =0x30a1a
	strh    r8, [r9]
	mov		r8,#1
	ldr		r9,=0x30400
	str		r8,[r9]
endFiq:
	@ ack the FIQ
	mov		r9,	#0xb
	mov     r8, #1
	mov     r8, r8, lsl r9
	ldr     r9, =0x30502
	ldrh    r10, [r9]
	orr     r10, r10, r8
	strh    r10, [r9]
avFIQout:
	subs pc, lr, #4

.avLOOPval1:	.word	0x0
.avLOOPval2:	.word	0x0        
        
        .thumb
        
@ ------------------------------------------------------------------------------
@ interruptsGetCauses()
.globl interruptsGetCausesA
interruptsGetCausesA:
        switchThumb
.globl interruptsGetCauses
.thumb_func

interruptsGetCauses:
        push {r1, r2}
        ldr r2, =0x30504
        ldrh r0, [r2, #2]
        lsl r0, #16
        mov r1, #0
        ldrh r1, [r2]
        orr r0, r1
        pop {r1, r2}
        bx lr

@ ------------------------------------------------------------------------------
@ interruptsSetCauses(r0=val)
.globl interruptsSetCausesA
interruptsSetCausesA:
        switchThumb
.globl interruptsSetCauses
.thumb_func

interruptsSetCauses:
        push {r0, r1}
        ldr r1, =0x30504
        strh r0, [r1]
        lsr r0, #16
        strh r0, [r1, #2]
        pop {r0, r1}
        bx lr

@ ------------------------------------------------------------------------------
@ interruptsGetCauses()
.globl interruptsGetCauses2A
interruptsGetCauses2A:
        switchThumb
.globl interruptsGetCauses2
.thumb_func

interruptsGetCauses2:
        push {r1, r2}
        ldr r2, =0x30500
        ldrh r0, [r2, #2]
        lsl r0, #16
        mov r1, #0
        ldrh r1, [r2]
        orr r0, r1
        pop {r1, r2}
        bx lr

@ ------------------------------------------------------------------------------
@ interruptsSetCauses2(r0=val)
.globl interruptsSetCauses2A
interruptsSetCauses2A:
        switchThumb
.globl interruptsSetCauses2
.thumb_func

interruptsSetCauses2:
        push {r0, r1}
        ldr r1, =0x30500
        strh r0, [r1]
        lsr r0, #16
        strh r0, [r1, #2]
        pop {r0, r1}
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
        push {r0, r1, r2}
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
        pop {r1, r2}
        bx lr
        
irl2:   ldrh r2, [r0]
        orr r2, r1
        strh r2, [r0]
        pop {r0, r1, r2}
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
        pop {r0, r1, r2}
        bx lr

irl3:   ldrh r2, [r0, #2]
        orr r2, r1
        strh r2, [r0, #2]
        pop {r0, r1, r2}
        bx lr

@ ------------------------------------------------------------------------------
@ interruptsResetMaskA(r0=val)
.globl interruptsResetMaskA
interruptsResetMaskA:
        switchThumb
.globl interruptsResetMask
.thumb_func

interruptsResetMask:
        push {r0, r1, r2}
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
        pop {r0, r1, r2}
        bx lr
        
        .arm
        .ltorg

.endif
