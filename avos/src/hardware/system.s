@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@
@ Date:     04/01/2004
@ Author:   By DoggerMoore
@
@

.ifndef systemInc
systemInc = 1

.text

        
@ ------------------------------------------------------------------------------
@ systemReboot()
@
.globl systemReboot
.thumb_func

systemReboot:
        ldr r0, =0x034000a5     @ BOOTLOADER 0.4a Address...
        bx r0
        bx lr           @ Never get here

@ ------------------------------------------------------------------------------
@ systemReset()
@
.globl systemReset
.thumb_func

systemReset:
        @ Looks like maybe we have to disable interrupts or some other stuff
        @ here? as it is the unit turns off instead of resetting :(

        mov r0, #0
        bx r0
        bx lr           @ Never get here
        
        .arm
        .ltorg

.endif
