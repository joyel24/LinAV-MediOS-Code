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

.ifndef uartInc
uartInc = 1

.text

        uartBase    =   0x30300     @ av300 UART1

        uartRegDTTR =   0x00
        uartRegBRSR =   0x02
        uartRegMSR  =   0x04
        uartRegRFCR =   0x06
        uartRegTFCR =   0x08
        uartRegLCR  =   0x0a
        uartRegSR   =   0x0c

        .thumb
@ ------------------------------------------------------------------------------
@ uartGetch()
@   Returns char in r0, or -1 if none to be read.
.globl uartGetch
.thumb_func

uartGetch:
        push {r1, r2, r3}
        ldr r2, =uartBase
        mov r1, #0
        ldrh r1, [r2, #uartRegRFCR]
        mov r0, #0
        sub r0, #1              @ assume failure...
        lsl r1, #26
         beq uartNon
        mov r0, #0
        ldrh r0, [r2, #uartRegDTTR]
uartNon:pop {r1, r2, r3}
        bx lr



@ ------------------------------------------------------------------------------
@ uartOutch r0=chr to output to UART1
@
.globl uartOutch
.thumb_func

uartOutch:
        push {r1, r2}
        ldr r1, =uartBase
uartWt1:ldrh r2, [r1, #uartRegSR]
        lsr r2, r2, #11
         bcc uartWt1
        strh r0, [r1, #uartRegDTTR]
        pop {r1, r2}
        bx lr

        
@ ------------------------------------------------------------------------------
@ uartOuts r0->string to output to UART1
@
.globl uartOuts
.thumb_func
uartOuts:
        push {r1, lr}
        mov r1, r0
uartLp1:ldrb r0, [r1, #0]
        cmp r0, #0
         beq uartEn1
        bl uartOutch
        add r1, r1, #1
        b uartLp1
uartEn1:pop {r1}
        pop {r1}
        bx r1

        .ltorg
        .arm

.endif
