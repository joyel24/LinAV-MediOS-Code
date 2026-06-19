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
        
        i2cSDA          =   0x08
        i2cSCL          =   0x04

        @ jbmm uses 0x30600, 0x0800, 0x1000


.macro SDA_LO
        mov r6, #i2cSDA
        ldrh r0, [r7, #i2cRegDR]
        bic r0, r6
        strh r0, [r7, #i2cRegDR]
.endm

.macro SDA_HI
        mov r6, #i2cSDA
        ldrh r0, [r7, #i2cRegDR]
        orr r0, r6
        strh r0, [r7, #i2cRegDR]
.endm

.macro SDA_INPUT
        mov r6, #i2cSDA
        ldrh r0, [r7, #i2cRegIO]
        orr r0, r6
        strh r0, [r7, #i2cRegIO]
.endm

.macro SDA_OUTPUT
        mov r6, #i2cSDA
        ldrh r0, [r7, #i2cRegIO]
        bic r0, r6
        strh r0, [r7, #i2cRegIO]
.endm

.macro SCL_LO
        mov r6, #i2cSCL
        ldrh r0, [r7, #i2cRegDR]
        bic r0, r6
        strh r0, [r7, #i2cRegDR]
.endm

.macro SCL_HI
        mov r6, #i2cSCL
        ldrh r0, [r7, #i2cRegDR]
        orr r0, r6
        strh r0, [r7, #i2cRegDR]
.endm

.macro SCL_INPUT
        mov r6, #i2cSCL
        ldrh r0, [r7, #i2cRegIO]
        orr r0, r6
        strh r0, [r7, #i2cRegIO]
.endm

.macro SCL_OUTPUT
        mov r6, #i2cSCL
        ldrh r0, [r7, #i2cRegIO]
        bic r0, r6
        strh r0, [r7, #i2cRegIO]
.endm

.text

        .thumb
@ ------------------------------------------------------------------------------
        
        
@ i2cStartT
@
.globl i2cStartT
.thumb_func
i2cStartT:
        push {r0, r6, r7, lr}
        ldr r7, =i2cBaseAddress
        SDA_OUTPUT
        SDA_HI
        SCL_HI
        SDA_LO
        mov r0, #255
i2cd1:  sub r0, #1
        cmp r0, #0
        bne i2cd1
        SCL_LO
        pop {r0, r6, r7, pc}

@ i2cStopT
@
.globl i2cStopT
.thumb_func
i2cStopT:
        push {r0, r6, r7, lr}
        ldr r7, =i2cBaseAddress
        SDA_LO
        SCL_HI
        mov r0, #255
i2cd2:  sub r0, #1
        cmp r0, #0
        bne i2cd2
        SDA_HI
        pop {r0, r6, r7, pc}
        bx lr

@ i2cInitT
@
.globl i2cInitT
.thumb_func
i2cInitT:
        push {r0, r1, r6, r7, lr}
        ldr r7, =i2cBaseAddress
        SDA_OUTPUT
        SDA_HI
        SCL_LO
        mov r1, #3
i2cl1:  callThumb r0, i2cStopT
        sub r1, #1
         bne i2cl1
        pop {r0, r1, r6, r7, pc}

@ i2cAckT
@
.globl i2cAckT
.thumb_func
i2cAckT: 
        push {r0, r1, r6, r7, lr}
        ldr r7, =i2cBaseAddress
        SCL_LO
        cmp r0, #1
         beq i2chi1
        SDA_LO
        b i2cad
i2chi1: SDA_HI
i2cad:
        SCL_INPUT
        SCL_HI                  @ MAY NOT BE NEEDED
        
        @ Wait for it to be released...
        mov r0, #0
i2cw1:  ldrh r0, [r7, #i2cRegIO]
        cmp r0, #0
         beq i2cw1

        mov r0, #255
i2cd3:  sub r0, #1
        cmp r0, #0
         bne i2cd3        
        SCL_OUTPUT
        SCL_LO
        pop {r0, r1, r6, r7, pc}

@ i2cGetAckT
@
.globl i2cGetAckT
.thumb_func
i2cGetAckT:
        push {r1, r2, r3, r6, r7, lr}
        ldr r7, =i2cBaseAddress
        
        SDA_INPUT
        SCL_INPUT
        SCL_HI                                  @ MAY NOT BE NEEDED

        mov r0, #0
i2cw2:  ldrh r0, [r7, #i2cRegIO]
        cmp r0, #0
         beq i2cw2

        mov r2, #1
        ldrh r3, [r7, #i2cRegDR]
        cmp r3, #0
         bne i2cok
        mov r2, #0   
i2cok:
        SCL_OUTPUT
        SCL_LO
        SDA_HI
        SDA_OUTPUT

        
        mov r0, r2
        pop {r1, r2, r3, r6, r7, pc}
        
@ i2cOutbT(byte)
@
.globl i2cOutbT
.thumb_func
i2cOutbT:
        push {r0, r1, r2, r3, r6, r7, lr}
        ldr r7, =i2cBaseAddress
        mov r3, r0
        mov r2, #0x80
i2ccl1:
        mov r1, r3
        and r1, r2
        cmp r1, #0
         bne i2cho
        SDA_LO
        b i2cdo
i2cho:  SDA_HI
i2cdo:
        SCL_HI
        SCL_LO

        lsr r2, #1
        cmp r2, #0
         bne i2ccl1
        SDA_HI
        pop {r0, r1, r2, r3, r6, r7, pc}

@ byte i2cInbT
@
.globl i2cInbT
.thumb_func
i2cInbT:
        push {r1, r2, r3, r6, r7, lr}
        ldr r7, =i2cBaseAddress
        
        mov r2, #0x80
        mov r1, #0
        mov r3, #0
i2ccl2:
        SDA_LO                          @ may be not needed...

        SDA_INPUT
        SCL_HI
        ldrh r1, [r7, #i2cRegDR]
        cmp r1, #0
         beq i2cnod
        orr r3, r2  
i2cnod:
        SCL_LO
        SDA_OUTPUT

        lsr r2, #1
        cmp r2, #0
         bne i2ccl2
        
        mov r0, #1
        callThumb r2, i2cGetAckT
        mov r0, r3
        pop {r1, r2, r3, r6, r7, pc}

@ i2cReadT (r0=addr, r1->buffer, r2 = count)
@
.globl i2cReadT
.thumb_func
i2cReadT:
        push {r1, r2, r3, r6, r7, lr}

        callThumb r3, i2cStartT
        mov r3, #1
        orr r0, r3
        callThumb r3, i2cOutbT

        callThumb r3, i2cGetAckT
        cmp r0, #0
         beq i2crError
        
i2clr:  callThumb r3, i2cInbT
        strb r0, [r1]
        add r1, #1
        sub r2, #1
        cmp r2, #0
         bne i2clr
         
        callThumb r3, i2cStopT
        mov r0, #0
        pop {r1, r2, r3, r6, r7, pc}
        
i2crError:
        callThumb r3, i2cStopT
        mov r0, #0
        sub r0, #1
        pop {r1, r2, r3, r6, r7, pc}

@ i2cWriteT (r0=addr, r1->buffer, r2 = count)
@
.globl i2cWriteT
.thumb_func
i2cWriteT:
        push {r1, r2, r3, r6, r7, lr}

        callThumb r3, i2cStartT
        mov r3, #0xfe
        and r0, r3
        callThumb r3, i2cOutbT

        callThumb r3, i2cGetAckT
        cmp r0, #0
         beq i2cwError

i2clw:  mov r0, #0
        ldrb r0, [r1]
        callThumb r3, i2cOutbT
        callThumb r3, i2cGetAckT
        cmp r0, #0
         beq i2cwError
        add r1, #1
        sub r2, #1
        cmp r2, #0
         bne i2clw

        callThumb r3, i2cStopT
        mov r0, #0
        pop {r1, r2, r3, r6, r7, pc}
        
i2cwError:
        callThumb r3, i2cStopT
        mov r0, #0
        sub r0, #1
        pop {r1, r2, r3, r6, r7, pc}
        
        .arm
        .ltorg
