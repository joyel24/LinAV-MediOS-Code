@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@ ExpansionPort
@
@ Date:     02/02/2004
@ Author:   By DoggerMoore
@
@ int expansionGetID()
@

.ifndef expansionPortInc
expansionPortInc = 1

.text
        .thumb

@ u32 expansionGetID() 
@       returns r0 = expansionModule ID
@
.globl expansionGetID
.thumb_func
expansionGetID:
        push {r1, lr}
        ldr r1, =0x02600000
        ldrh r0, [r1]
        lsl r0, #28
        lsr r0, #28
        pop {r1, pc}

        .arm
        .ltorg
.endif
