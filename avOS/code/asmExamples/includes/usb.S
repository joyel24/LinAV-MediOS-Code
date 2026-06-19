@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@ usb
@
@ Date:     15/01/2004
@ Author:   By DoggerMoore
@
@ u32 usbIsConnected()
@ void usbEnable()
@ void usbDisable()
@

.ifndef usbInc
usbInc = 1

.text

        .thumb

@ ------------------------------------------------------------------------------
@ usbIsConnected
@   returns 0/1 in r0
.globl usbIsConnected
.thumb_func

usbIsConnected:
        push {r1, lr}
        ldr r1, =0x30a24
        ldrh r0, [r1]
        lsr r0, #6
        mov r1, #1
        and r0, r1
        pop {r1, pc}

        
@ ------------------------------------------------------------------------------
@ usbEnable
@
.globl usbEnable
.thumb_func

usbEnable:
        push {r1, lr}
        ldr r1, =0x02600100
        mov r0, #1
        strh r0, [r1]
        pop {r1, pc}


@ ------------------------------------------------------------------------------
@ usbDisable
@
.globl usbDisable
.thumb_func

usbDisable:
        push {r1, lr}
        ldr r1, =0x02600100
        mov r0, #0
        strh r0, [r1]
        pop {r1, pc}
        
        .arm
        .ltorg
        
.endif
