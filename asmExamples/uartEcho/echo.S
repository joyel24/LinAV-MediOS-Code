@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@ echo
@
@ Date:     04/01/2004
@ Author:   By DoggerMoore
@
@

.text

loop1:

        add r0, pc, #1
        bx r0                   @ switch to thumb
        
        .thumb

mloop:
        ldr r1, =(uartGetchT+1)
        mov r0, #1
        mov lr, r0
        add lr, pc
        bx r1
        ldr r1, =(-1)
        cmp r0, r1
        beq mloop               @ no input!
        
        ldr r1, =(uartOutchT+1)
        mov r2, #1
        mov lr, r2
        add lr, pc
        bx r1                   @ Right back atcha!

        b mloop

.include "../includes/uart.S"
