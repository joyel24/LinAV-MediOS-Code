@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@ I2C
@
@ This interfaces with I2C bus.
@
@ Date:     13/01/2004
@ Author:   By DoggerMoore
@
@ i2cStartT
@ i2cStopT
@ i2cInitT
@ i2cAckT (r0=0/1)
@
        
        i2cBaseAddress  =   0x30580
        
        i2cRegDR        =   0x02
        i2cRegIO        =   0x0e
        
        i2cSDA          =   0x04
        i2cSCL          =   0x08

        @ jbmm uses 0x30600, 0x0800, 0x1000


.macro SDA_LO
        ldrh r0, [r7, #i2cRegDR]
        bic r0, #i2cSDA
        strh r0, [r7, #i2cRegDR]
.endm

.macro SDA_HI
        ldrh r0, [r7, #i2cRegDR]
        orr r0, #i2cSDA
        strh r0, [r7, #i2cRegDR]
.endm

.macro SDA_INPUT
        ldrh r0, [r7, #i2cRegIO]
        bic r0, #i2cSDA
        strh r0, [r7, #i2cRegIO]
.endm

.macro SDA_OUTPUT
        ldrh r0, [r7, #i2cRegIO]
        orr r0, #i2cSDA
        strh r0, [r7, #i2cRegIO]
.endm

.macro SCL_LO
        ldrh r0, [r7, #i2cRegDR]
        bic r0, #i2cSCL
        strh r0, [r7, #i2cRegDR]
.endm

.macro SCL_HI
        ldrh r0, [r7, #i2cRegDR]
        orr r0, #i2cSCL
        strh r0, [r7, #i2cRegDR]
.endm

.macro SCL_INPUT
        ldrh r0, [r7, #i2cRegIO]
        bic r0, #i2cSCL
        strh r0, [r7, #i2cRegIO]
.endm

.macro SCL_OUTPUT
        ldrh r0, [r7, #i2cRegIO]
        orr r0, #i2cSCL
        strh r0, [r7, #i2cRegIO]
.endm

.text

        .thumb

@ i2cStartT
@
i2cStartT:
        push {r0, r7}
        ldr r7, =i2cBaseAddress
        SDA_INPUT
        SDA_HI
        SCL_HI
        SDA_LO
        mov r0, #255
i2cd1:  sub r0, #1
        bne i2cd1
        SCL_LO
        pop {r0, r7}
        bx lr

@ i2cStopT
@
i2cStopT:
        push {r0, r7}
        ldr r7, =i2cBaseAddress
        SDA_LO
        SCL_HI
        mov r0, #255
i2cd2:  sub r0, #1
        bne i2cd2        
        SDA_HI
        pop {r0, r7}
        bx lr

@ i2cInitT
@
i2cInitT:
        push {r0, r1, r7, lr}
        ldr r7, =i2cBaseAddress
        SDA_OUTPUT
        SDA_HI
        SCL_LO
        mov r1, #3
i2cl1:  callThumb r0, i2cStopT
        sub r1, #1
         bne i2cl1
        pop {r0, r1, r7, pc}

@ i2cAckT
@
i2cAckT: 
        push {r0, r1, r7}
        ldr r7, =i2cBaseAddress
        SCL_LO
        cmp r0, #1
        beq i2chi1
        SDA_LO
        b i2cad
i2chi1: SDA_HI
i2cad:
        SCL_INPUT
        
        @ Wait for it to be released...
        mov r0, #0
i2cw1:  ldrh r0, [r7, #i2cRegIO]
        cmp r0, #0
        beq i2cw1
        
        mov r0, #255
i2cd3:  sub r0, #1
        bne i2cd3        
        SCL_OUTPUT
        SCL_LO
        pop {r0, r1, r7}
        bx lr
        
        
        .arm
        .ltorg
