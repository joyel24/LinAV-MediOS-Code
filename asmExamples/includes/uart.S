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
@ Date:     04/01/2004
@ Author:   By DoggerMoore
@
@ uartOutchT (r0=character)
@ uartOutsT (r0->string)
@ uartGetchT    returns a char in r0, or -1 if none waiting.
@
@ NOTE: at the moment only supports UART1, but then thats the only
@       useful one as yet...

.ifndef uartInc
uartInc = 1

.text

        uartBase    =   0x30300     @ av300 UART1

        @ 0 DTRR    Data xfer
        @ 2 BRSR    Bitrate set
        @ 4 MSR     Mode set
        @ 6 RFCR    Recv FIFO
        @ 8 TFCR    Trans FIFO
        @ a LCR     Line control
        @ c SR      Status reg
        
        .thumb

@ uartGetchT returns r0=chr or -1 if none
@
uartGetchT:
        push {r1, r2, r3, lr}
        ldr r2, =uartBase
        mov r1, #0
        ldrh r1, [r2, #6]
        ldr r0, =(-1)           @ assume failure...
        lsl r1, #26
        beq uartNon
        mov r0, #0
        ldrh r0, [r2, #0]
uartNon:
        pop {r1, r2, r3}
        pop {r1}
        bx r1

@ uartOutchT r0=chr to output to UART1
@
uartOutchT:
        push {r1, r2}
        ldr r1, =uartBase
uartWait1:
        ldrh r2, [r1, #0xc]
        lsr r2, r2, #11
         bcc uartWait1

        strh r0, [r1, #0x0]
        pop {r1, r2}
        bx lr

@ uartOutsT r0->string to output to UART1
@
uartOutsT:
        push {r2, lr}
        mov r1, r0
uartLoop1:
        ldrb r0, [r1, #0]
        cmp r0, #0
         beq uartEnd1
        mov r2, #1
        mov lr, r2
        ldr r2, =(uartOutchT+1)
        add lr, pc
        bx r2
        add r1, r1, #1
        b uartLoop1
uartEnd1:
        pop {r2}
        pop {r0}
        bx r0

        .ltorg
        .arm
        
.endif
