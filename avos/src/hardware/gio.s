@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@ gio
@
@ Date:     04/01/2004
@ Author:   By DoggerMoore
@
@ void gioSetDirections(r0=directions)
@ u32 gioGetDirections()
@ void gioSetInverts(r0=inverts)
@ u32 gioGetInverts()
@ void gioSetBitsets(r0=bitsets)
@ u32 gioGetBitsets()
@ void gioSetBitclears(r0=bitclears)
@ u32 gioGetBitclears()
@
@ void gioSetBit(r0=line)
@ void gioClearBit(r0=line)
@ u32 gioGetBit(r0=line)
@

    GIO_OUTPUT      =   0
    GIO_INPUT       =   1
    GIO_UNINVERTED  =   0
    GIO_INVERTED    =   1
    
.macro switchThumb
        .arm
        add ip, pc, #1
        bx ip
        .thumb
.endm

.ifndef gioInc
gioInc = 1

.text

        
@ ------------------------------------------------------------------------------
@ gioSetAllDirections(r0=directions)
.globl gioSetAllDirectionsA
gioSetAllDirectionsA:
        switchThumb
.globl gioSetAllDirections
.thumb_func

gioSetAllDirections:
        push {r0, r2}
        ldr r2, =0x30580
        strh r0, [r2]
        lsr r0, #16
        strh r0, [r2, #2]
        pop {r0, r2}
        bx lr


@ ------------------------------------------------------------------------------
@ gioGetAllDirections()
@
.globl gioGetAllDirectionsA
gioGetAllDirectionsA:
        switchThumb
.globl gioGetAllDirections
.thumb_func

gioGetAllDirections:
        push {r1, r2}
        ldr r2, =0x30580
        ldrh r0, [r2, #2]
        lsl r0, #16
        mov r1, #0
        ldrh r1, [r2]
        orr r0, r1
        pop {r1, r2}
        bx lr


@ ------------------------------------------------------------------------------
@ gioSetAllInverts(r0=invert)
.globl gioSetAllInvertsA
gioSetAllInvertsA:
        switchThumb
.globl gioSetAllInverts
.thumb_func

gioSetAllInverts:
        push {r0, r2}
        ldr r2, =0x30584
        strh r0, [r2]
        lsr r0, #16
        strh r0, [r2, #2]
        pop {r0, r2}
        bx lr


@ ------------------------------------------------------------------------------
@ gioGetAllInverts()
@
.globl gioGetAllInvertsA
gioGetAllInvertsA:
        switchThumb
.globl gioGetAllInverts
.thumb_func

gioGetAllInverts:
        push {r1, r2}
        ldr r2, =0x30584
        ldrh r0, [r2, #2]
        lsl r0, #16
        mov r1, #0
        ldrh r1, [r2]
        orr r0, r1
        pop {r1, r2}
        bx lr

@ ------------------------------------------------------------------------------
@ gioSetAllBitsets(r0=bitsets)
.globl gioSetAllBitsetsA
gioSetAllBitsetsA:
        switchThumb
.globl gioSetAllBitsets
.thumb_func

gioSetAllBitsets:
        push {r0, r2}
        ldr r2, =0x30588
        strh r0, [r2]
        lsr r0, #16
        strh r0, [r2, #2]
        pop {r0, r2}
        bx lr


@ ------------------------------------------------------------------------------
@ gioGetAllBitsets()
@
.globl gioGetAllBitsetsA
gioGetAllBitsetsA:
        switchThumb
.globl gioGetAllBitsets
.thumb_func

gioGetAllBitsets:
        push {r1, r2}
        ldr r2, =0x30588
        ldrh r0, [r2, #2]
        lsl r0, #16
        mov r1, #0
        ldrh r1, [r2]
        orr r0, r1
        pop {r1, r2}
        bx lr
        
@ ------------------------------------------------------------------------------
@ gioSetAllBitclears(r0=bitclears)
.globl gioSetAllBitclearsA
gioSetAllBitclearsA:
        switchThumb
.globl gioSetAllBitclears
.thumb_func

gioSetAllBitclears:
        push {r0, r2}
        ldr r2, =0x3058c
        strh r0, [r2]
        lsr r0, #16
        strh r0, [r2, #2]
        pop {r0, r2}
        bx lr


@ ------------------------------------------------------------------------------
@ gioGetAllBitclears()
@
.globl gioGetAllBitclearsA
gioGetAllBitclearsA:
        switchThumb
.globl gioGetAllBitclears
.thumb_func

gioGetAllBitclears:
        push {r1, r2}
        ldr r2, =0x3058c
        ldrh r0, [r2, #2]
        lsl r0, #16
        mov r1, #0
        ldrh r1, [r2]
        orr r0, r1
        pop {r1, r2}
        bx lr
        

@ ------------------------------------------------------------------------------
@ gioSetBit(r0=line)
@
.globl gioSetBitA
gioSetBitA:
        switchThumb
.globl gioSetBit
.thumb_func

gioSetBit:
        push {r1, r2, lr}
        mov r1, #1
        lsl r1, r0
        mov r0, r1
        bl gioSetAllBitsets
        pop {r1, r2}
        pop {r1}
        bx r1

@ ------------------------------------------------------------------------------
@ gioClearBit(r0=line)
@
.globl gioClearBitA
gioClearBitA:
        switchThumb
.globl gioClearBit
.thumb_func

gioClearBit:
        push {r1, r2, lr}
        mov r1, #1
        lsl r1, r0
        mov r0, r1
        bl gioSetAllBitclears
        pop {r1, r2}
        pop {r1}
        bx r1

        
@ ------------------------------------------------------------------------------
@ gioGetBit(r0=line)
@
.globl gioGetBitA
gioGetBitA:
        switchThumb
.globl gioGetBit
.thumb_func

gioGetBit:
        push {r1, r2, lr}
        mov r2, r0
        bl gioGetAllBitsets
        lsr r0, r2
        lsl r0, #31
        lsr r0, #31
        pop {r1, r2}
        pop {r1}
        bx r1
        
        .arm
        .ltorg

.endif
