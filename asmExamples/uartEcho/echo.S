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
.include "../includes/common.S"

.text

        add r0, pc, #1
        bx r0                   @ switch to thumb
        
        .thumb
        
mloop:
        callThumb r1, uartGetchT
        ldr r1, =(-1)
        cmp r0, r1
        beq mloop               @ no input!
        
        callThumb r1, uartOutchT

        b mloop

.include "../includes/uart.S"

        .thumb

