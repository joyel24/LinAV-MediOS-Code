@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@ Battery - Get battery info
@
@ Date:     15/01/2004
@ Author:   By DoggerMoore
@
@ batteryIsDCT  returns 0/1 in r0 depending on if charger is plugged in.
@

.ifndef batteryInc
batteryInc = 1

.text

        .thumb

@ batteryIsDCT
@
.globl batteryIsDCT
.thumb_func
batteryIsDCT:
        push {r1}
        ldr r1, =0x30a24
        ldrh r0, [r1]
        lsr r0, #5
        mov r1, #1
        and r0, r1
        pop {r1}
        bx lr

        .arm
        .ltorg
        
.endif
