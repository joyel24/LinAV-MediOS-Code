@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@ CPLD
@
@ Date:     29/03/2004
@ Author:   By DoggerMoore
@
@ void cpldSetModeA()
@

.macro switchThumb
        .arm
        add ip, pc, #1
        bx ip
        .thumb
.endm

.ifndef cpldInc
cpldInc = 1

.text

        .thumb

@ ------------------------------------------------------------------------------
@ cpldSetModeA(u32 mode)
@   Selects 
.globl cpldSetModeA
cpldSetModeA:
        switchThumb
.globl cpldSetMode
.thumb_func

cpldSetMode:
        push {r1}
        ldr r1, =0x02600300
        strh r0, [r1]       @ Not sure why its done 5 times?
        strh r0, [r1]
        strh r0, [r1]
        strh r0, [r1]
        strh r0, [r1]
        pop {r1}
        bx lr

        .ltorg
        .arm
        
.endif
