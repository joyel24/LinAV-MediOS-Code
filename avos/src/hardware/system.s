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
@ systemRelocateMe()
@
.globl systemRelocateMe
.thumb_func

systemRelocateMe:
        ldr r0, =0x03000000     @ src
        ldr r1, =0x03400000     @ dest
        ldr r3, =0x00080000     @ Size (Fixed atm)...
srmc:   ldr r2, [r0]
        str r2, [r1]
        add r0, #4
        add r1, #4
        sub r3, #4
         bne srmc
        mov r0, lr
        ldr r1, =0x00400000
        add r0, r1
        bx r0                   @ Jump back, but at higher address...

        
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
