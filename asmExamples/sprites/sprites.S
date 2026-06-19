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
.include "../includes/jbmm.S"

.text

loop1:
        add r0, pc, #1
        bx r0                   @ switch to thumb
        
        .thumb

        callThumb r0, lcdInitT
       
        mov r0, #0
        mov r1, #0
        ldr r2, =320
        ldr r3, =240
        ldr r4, =sprite
        callThumb r5, lcdSpriteT
        
mloop:
        b mloop

.include "../includes/lcd.S"

sprite:
.include "sprite.inc"

