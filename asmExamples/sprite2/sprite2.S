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

loop1:
        add r0, pc, #1
        bx r0                   @ switch to thumb
        
        .thumb

        callThumb r0, lcdInitT

        ldr r0, =lcdVideoBuffer
        ldr r1, =sprite
        ldr r2, =(320*240)
lcop:   ldr r3, [r1]
        str r3, [r0]
        add r1, #4
        add r0, #4
        sub r2, #1
        bne lcop
mloop:
        b mloop

.include "../includes/lcd.S"

sprite:
.include "sprite.inc"

