@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@ Buttons - Get button info
@
@ This interfaces with the buttons on the AV300.
@
@ Date:     04/01/2004
@ Author:   By DoggerMoore
@
@ buttonsGetStatusT
@   - Returns button status in r0

.ifndef buttonsInc
buttonsInc = 1

.text

@
@ getButtonStatus - returns code in r0.
@ up    (slow)       1
@ left  (slow)       2
@ right (slow)      10
@ down  (slow)      20
@ menu1            100 
@ menu2            200
@ menu3            040
@ joypress/fast    400 

@ TODO - on/off buttons:
@   30588/c bit 0x01
@   3058a/e but 0x10

        .thumb

@ buttonsGetStatusT
@
.globl buttonsGetStatusT
.thumb_func
buttonsGetStatusT:
        push {r1, r2, r3}
        mov r3, #0x0f
        ldr r2, =0x2600680
        ldrh r1, [r2, #0]
        and r1, r3
        mov r0, r1

        ldr r2, =0x2600700
        ldrh r1, [r2, #0]
        and r1, r3
        lsl r1, r1, #4
        orr r0, r1
        
        ldr r2, =0x2600780
        ldrh r1, [r2, #0]
        and r1, r3
        lsl r1, r1, #8
        orr r0, r1
        pop {r1, r2, r3}
        bx lr                   @ incase its comming from arm code.

        .arm
        .ltorg

.endif
