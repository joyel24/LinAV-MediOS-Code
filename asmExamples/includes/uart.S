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
@
@ NOTE: at the moment only supports UART1, but then thats the only
@       useful one as yet...

.text

        .thumb

@ uartOutchT r0=chr to output to UART1
@
uartOutchT:
        push {r1, r2}
        ldr r1, =0x30300        @ UART1 base
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
