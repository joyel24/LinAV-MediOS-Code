@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@ Uart - useful uart routines...
@
@
@ Date:     14/02/2004
@ Author:   By DoggerMoore
@

.macro switchThumb
        .arm
        add ip, pc, #1
        bx ip
        .thumb
.endm

.ifndef uartInc
uartInc = 1

.text
        
        uartRegDTTR =   0x00
        uartRegBRSR =   0x02
        uartRegMSR  =   0x04
        uartRegRFCR =   0x06
        uartRegTFCR =   0x08
        uartRegLCR  =   0x0a
        uartRegSR   =   0x0c

        .thumb
@ ------------------------------------------------------------------------------
@ uartGetch(r0=device)
@   Returns char in r0, or -1 if none to be read.
.globl uartGetchA
uartGetchA:
        switchThumb
.globl uartGetch
.thumb_func

uartGetch:
        lsl r0, #2
        ldr r2, =uartBases
        ldr r2, [r2, r0]
        mov r1, #0
        ldrh r1, [r2, #uartRegRFCR]
        mov r0, #0
        sub r0, #1              @ assume failure...
        lsl r1, #26
         beq uartNon
        mov r0, #0
        ldrh r0, [r2, #uartRegDTTR]
uartNon:bx lr



@ ------------------------------------------------------------------------------
@ uartOutch r0=device, r1=chr to output to UART
@
.globl uartOutchA
uartOutchA:
        switchThumb
.globl uartOutch
.thumb_func

uartOutch:
        lsl r0, #2
        ldr r2, =uartBases
        ldr r2, [r2, r0]
uartWt1:ldrh r3, [r2, #uartRegSR]
        lsr r3, r3, #11
         bcc uartWt1
        strh r1, [r2, #uartRegDTTR]
        bx lr

        
@ ------------------------------------------------------------------------------
@ uartOuts r0=device, r1->string to output to UART1
@
.globl uartOutsA
uartOutsA:
        switchThumb
.globl uartOuts
.thumb_func
uartOuts:
        push {lr}
        mov r2, r1
uartLp1:ldrb r1, [r2, #0]
        cmp r1, #0
         beq uartEn1
        push {r0, r2}
        bl uartOutch
        pop {r0, r2}
        add r2, #1
        b uartLp1
uartEn1:pop {r1}
        bx r1

        .ltorg
        .arm

uartBases:  .word 30300
            .word 30380
        
.endif
