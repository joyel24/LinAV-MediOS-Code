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
@ 
@ i2cWriteRaw(device, buffer, count)
@ i2cWrite(device, addr, buffer, count)
@ i2cRead(device, addr, buffer, count)
@ i2cOutb(data)
@ i2cInb()
@ i2cGetAck()
@ i2cStop()
@ i2cStart()
@ i2cAck()
@ i2cAckEnd()
@ 
        
.macro switchThumb
        .arm
        add ip, pc, #1
        bx ip
        .thumb
.endm

        i2cBaseAddress  =   0x30580
        
        i2cRegDR        =   0x02
        i2cRegIN        =   0x0a
        i2cRegIO        =   0x0e
   
.text
        .thumb

@-------------------------------------------------------------------------------
@ i2cDelay
@
.thumb_func
i2cDelay:
        mov r0, #8
i2cDE:  sub r0, #1
         bne i2cDE
        bx lr

@-------------------------------------------------------------------------------
@
@
.globl i2cAckA
i2cAckA:
        switchThumb
.globl i2cAck
.thumb_func
i2cAck:
        push {r0, r1, r7, lr}
        ldr r7, =i2cBaseAddress
        
        mov r0, #4
        strh r0, [r7, #i2cRegIO]

        ldrh r0, [r7, #i2cRegDR]
        ldr r1, =0xfffb
        and r0, r1
        strh r0, [r7, #i2cRegDR]

        bl i2cDelay
        
        mov r0, #8
        strh r0, [r7, #i2cRegIO]

        ldrh r0, [r7, #i2cRegDR]
        ldr r1, =0xfff7
        and r0, r1
        strh r0, [r7, #i2cRegDR]
        
        bl i2cDelay
        
        ldrh r0, [r7, #i2cRegDR]
        mov r1, #4
        orr r0, r1
        strh r0, [r7, #i2cRegDR]
        
        bl i2cDelay
        
        ldrh r0, [r7, #i2cRegIN]
        lsr r0, #3
         bcs i2cXA2
        
i2cXA1: ldrh r0, [r7, #i2cRegIN]
        lsr r0, #3
         bcc i2cXA1   
i2cXA2:

        mov r0, #4
        strh r0, [r7, #i2cRegIO]

        ldrh r0, [r7, #i2cRegDR]
        ldr r1, =0xfffb
        and r0, r1
        strh r0, [r7, #i2cRegDR]

        bl i2cDelay
        
        ldrh r0, [r7, #i2cRegDR]
        mov r1, #8
        orr r0, r1
        strh r0, [r7, #i2cRegDR]
        
        pop {r0, r1, r7}
        pop {r1}
        bx r1
        
@-------------------------------------------------------------------------------
@ i2cStop
@
.globl i2cStopA
i2cStopA:
        switchThumb
.globl i2cStop
.thumb_func

i2cStop:
        push {r0, r1, r7, lr}
        ldr r7, =i2cBaseAddress

        mov r0, #4
        strh r0, [r7, #i2cRegIO]
        
        ldrh r0, [r7, #i2cRegDR]
        ldr r1, =0xfffb
        and r0, r1
        strh r0, [r7, #i2cRegDR]

        bl i2cDelay
        
        mov r0, #8
        strh r0, [r7, #i2cRegIO]
        
        ldrh r0, [r7, #i2cRegDR]
        ldr r1, =0xfff7
        and r0, r1
        strh r0, [r7, #i2cRegDR]
        
        bl i2cDelay
        
        ldrh r0, [r7, #i2cRegDR]
        mov r1, #4
        orr r0, r1
        strh r0, [r7, #i2cRegDR]
        
        bl i2cDelay
        
        ldrh r0, [r7, #i2cRegDR]
        mov r1, #8
        orr r0, r1
        strh r0, [r7, #i2cRegDR]
        pop {r0, r1, r7}
        pop {r1}
        bx r1
        
@-------------------------------------------------------------------------------
@
@
.globl i2cAckEndA
i2cAckEndA:
        switchThumb
.globl i2cAckEnd
.thumb_func
i2cAckEnd:
        push {r0, r1, r7, lr}
        ldr r7, =i2cBaseAddress
        
        mov r0, #4
        strh r0, [r7, #i2cRegIO]
        
        ldrh r0, [r7, #i2cRegDR]
        ldr r1, =0xfffb
        and r0, r1
        strh r0, [r7, #i2cRegDR]

        bl i2cDelay
        
        ldrh r0, [r7, #i2cRegDR]
        mov r1, #8
        orr r0, r1
        strh r0, [r7, #i2cRegDR]
        
        bl i2cDelay
        
        ldrh r0, [r7, #i2cRegDR]
        mov r1, #4
        orr r0, r1
        strh r0, [r7, #i2cRegDR]
        
        bl i2cDelay
        
        ldrh r0, [r7, #i2cRegIN]
        lsr r0, #3
         bcs i2cVA2
        
i2cVA1: ldrh r0, [r7, #i2cRegIN]
        lsr r0, #3
         bcc i2cVA1   
i2cVA2:
        bl i2cDelay
        
        mov r0, #4
        strh r0, [r7, #i2cRegIO]
        
        ldrh r0, [r7, #i2cRegDR]
        ldr r1, =0xfffb
        and r0, r1
        strh r0, [r7, #i2cRegDR]

        bl i2cDelay

        pop {r0, r1, r7}
        pop {r1}
        bx r1
        
@-------------------------------------------------------------------------------
@
@
.globl i2cStartA
i2cStartA:
        switchThumb
.globl i2cStart
.thumb_func
i2cStart:
        push {r0, r1, r7, lr}
        ldr r7, =i2cBaseAddress

        bl i2cDelay
        
        ldrh r0, [r7, #i2cRegDR]
        mov r1, #4
        orr r0, r1
        strh r0, [r7, #i2cRegDR]
        
        mov r0, #8
        strh r0, [r7, #i2cRegIO]
        
        ldrh r0, [r7, #i2cRegDR]
        ldr r1, =0xfff7
        and r0, r1
        strh r0, [r7, #i2cRegDR]

        bl i2cDelay
        
        mov r0, #8
        strh r0, [r7, #i2cRegIO]
        
        ldrh r0, [r7, #i2cRegDR]
        ldr r1, =0xfffb
        and r0, r1
        strh r0, [r7, #i2cRegDR]
        pop {r0, r1, r7}
        pop {r1}
        bx r1
        
@-------------------------------------------------------------------------------
@ i2cGetAck
@
.globl i2cGetAckA
i2cGetAckA:
        switchThumb
.globl i2cGetAck
.thumb_func

i2cGetAck:
        push {r1, r2, r3, r4, r7, lr}
        ldr r7, =i2cBaseAddress

        mov r0, #4
        strh r0, [r7, #i2cRegIO]

        ldrh r0, [r7, #i2cRegDR]
        ldr r1, =0xfffb
        and r0, r1
        strh r0, [r7, #i2cRegDR]
        
        bl i2cDelay
        
        ldrh r0, [r7, #i2cRegDR]
        mov r1, #8
        orr r0, r1
        strh r0, [r7, #i2cRegDR]
        
        bl i2cDelay
        
        ldrh r0, [r7, #i2cRegDR]
        mov r1, #4
        orr r0, r1
        strh r0, [r7, #i2cRegDR]
       
        bl i2cDelay
        
        ldrh r0, [r7, #i2cRegIN]
        lsr r0, #3
         bcs i2cGA2
        
i2cGA1: ldrh r0, [r7, #i2cRegIN]
        lsr r0, #3
         bcc i2cGA1   
i2cGA2:
        bl i2cDelay
        
        ldrh r0, [r7, #i2cRegIN]
        lsl r0, #28
        lsr r4, r0, #31
        
        bl i2cDelay

        mov r0, #4
        strh r0, [r7, #i2cRegIO]
        
        ldrh r0, [r7, #i2cRegDR]
        ldr r1, =0xfffb
        and r0, r1
        strh r0, [r7, #i2cRegDR]
        
        bl i2cDelay
        
        mov r0, r4
        pop {r1, r2, r3, r4, r7}
        pop {r1}
        bx r1

@-------------------------------------------------------------------------------
@ i2cInb
@
.globl i2cInbA
i2cInbA:
        switchThumb
.globl i2cInb
.thumb_func

i2cInb:
        push {r1, r2, r3, r4, r5, r6, r7, lr}

        ldr r7, =i2cBaseAddress
        mov r2, #0                      @ ACC
        mov r4, #8                      @ bit count...
i2cIL:
        mov r6, #4
        strh r6, [r7, #i2cRegIO]
        
        ldrh r6, [r7, #i2cRegDR]
        ldr r3, =0xfffb
        and r6, r3
        strh r6, [r7, #i2cRegDR]

        bl i2cDelay        

        ldrh r6, [r7, #i2cRegDR]
        mov r3, #4
        orr r6, r3
        strh r6, [r7, #i2cRegDR]
        
        bl i2cDelay
        
        ldrh r6, [r7, #i2cRegIN]
        lsr r6, #3
         bcs i2cW9
i2cW8:  ldrh r6, [r7, #i2cRegIN]
        lsr r0, #3
         bcc i2cW8        
i2cW9:  
        bl i2cDelay

        ldrh r0, [r7, #i2cRegIN]
        lsl r0, #28
        lsr r0, #31
        lsl r2, #1
        orr r2, r0
        
        bl i2cDelay
     
        sub r4, #1
         bne i2cIL
        
        mov r0, #4
        strh r0, [r7, #i2cRegIO]
        
        ldrh r0, [r7, #i2cRegDR]
        ldr r1, =0xfffb
        and r0, r1
        strh r0, [r7, #i2cRegDR]

        bl i2cDelay        
        
        mov r0, r2

        pop {r1, r2, r3, r4, r5, r6, r7}
        pop {r1}
        bx r1
        
@-------------------------------------------------------------------------------
@ i2cOutb (r0=data)
@
.globl i2cOutbA
i2cOutbA:
        switchThumb
.globl i2cOutb
.thumb_func

i2cOutb:
        push {r1, r2, r3, r4, r5, r6, r7, lr}
        
        ldr r7, =i2cBaseAddress
        mov r2, r0
        mov r4, #8                      @ bit count...
i2cOL:
        mov r6, #4
        strh r6, [r7, #i2cRegIO]
        
        ldrh r6, [r7, #i2cRegDR]
        ldr r3, =0xfffb
        and r6, r3
        strh r6, [r7, #i2cRegDR]

        bl i2cDelay     

        lsr r0, r2, #8
         bcs noHI
        mov r0, #8
        strh r0, [r7, #i2cRegIO]
        ldrh r0, [r7, #i2cRegDR]
        ldr r1, =0xfff7
        and r0, r1
        strh r0, [r7, #i2cRegDR]
        b i2cDP
noHI:
        mov r0, #4                      @@
        strh r0, [r7, #i2cRegIO]        @@
        ldrh r0, [r7, #i2cRegDR]
        mov r1, #8
        orr r0, r1
        strh r0, [r7, #i2cRegDR]
i2cDP:
        bl i2cDelay

        ldrh r6, [r7, #i2cRegDR]
        mov r5, #4
        orr r6, r5
        strh r6, [r7, #i2cRegDR]

        bl i2cDelay
        
        ldrh r6, [r7, #i2cRegIN]
        lsr r6, #3
         bcs i2cW2

i2cW1:  ldrh r6, [r7, #i2cRegIN]
        lsr r0, #3
         bcc i2cW1        
i2cW2:  

        bl i2cDelay

        lsl r0, r2, #25
        lsr r2, r0, #24
        sub r4, #1
         bne i2cOL

        mov r0, #4
        strh r0, [r7, #i2cRegIO]
        
        ldrh r0, [r7, #i2cRegDR]
        ldr r1, =0xfffb
        and r0, r1
        strh r0, [r7, #i2cRegDR]

        bl i2cDelay       
         
        bl i2cGetAck

        pop {r1, r2, r3, r4, r5, r6, r7}
        pop {r1}
        bx r1
        
@-------------------------------------------------------------------------------
@ i2cRead (r0=device, r1=addr, r2->buffer, r3=count)
@
.globl i2cReadA
i2cReadA:
        switchThumb
.globl i2cRead
.thumb_func

i2cRead:
        push {r1, r2, r3, r4, r5, r6, r7, lr}
        mov r4, r0

        ldr r7, =i2cBaseAddress
        ldrh r6, [r7, #i2cRegDR]
        mov r5, #4
        orr r6, r5
        strh r6, [r7, #i2cRegDR]

        ldrh r6, [r7, #i2cRegDR]
        mov r5, #8
        orr r6, r5
        strh r6, [r7, #i2cRegDR]

        ldrh r6, [r7, #i2cRegIN]
        lsr r6, #3
         bcc i2cRE

        ldrh r6, [r7, #i2cRegIN]
        lsr r6, #4
         bcc i2cRE
      
        mov r5, r1
        bl i2cStart
        
        lsl r0, #24
        lsr r0, #25
        lsl r0, #1
        bl i2cOutb
        cmp r0, #0
         bne i2cRE
         
        lsl r0, r5, #24
        lsr r0, #24
        bl i2cOutb
        cmp r0, #0
         bne i2cRE
        
        bl i2cStart
        
        lsl r0, r4, #24
        lsr r0, #24
        mov r6, #1
        orr r0, r6
        bl i2cOutb
        cmp r0, #0
         bne i2cRE
        
        cmp r3, #2
         blt i2cRN
        sub r3, #1
i2cRL:  bl i2cInb
        strb r0, [r2]
        add r2, #1
        
        bl i2cAck
        sub r3, #1
         bne i2cRL
i2cRN:
        bl i2cInb
        strb r0, [r2]
        
        bl i2cAckEnd

        bl i2cStop

        mov r0, #0
        pop {r1, r2, r3, r4, r5, r6, r7}
        pop {r1}
        bx r1
        
i2cRE:  mov r0, #0
        sub r0, #1
        pop {r1, r2, r3, r4, r5, r6, r7}
        pop {r1}
        bx r1


@-------------------------------------------------------------------------------
@ i2cWrite (r0=device, r1=addr, r2->buffer, r3=count)
@
.globl i2cWriteA
i2cWriteA:
        switchThumb
.globl i2cWrite
.thumb_func

i2cWrite:
        push {r1, r2, r3, r4, r5, r6, r7, lr}
        mov r4, r0
        
        ldr r7, =i2cBaseAddress
        ldrh r6, [r7, #i2cRegDR]
        mov r5, #4
        orr r6, r5
        strh r6, [r7, #i2cRegDR]

        ldrh r6, [r7, #i2cRegDR]
        mov r5, #8
        orr r6, r5
        strh r6, [r7, #i2cRegDR]

        ldrh r6, [r7, #i2cRegIN]
        lsr r6, #3
         bcc i2cWE

        ldrh r6, [r7, #i2cRegIN]
        lsr r6, #4
         bcc i2cWE

        mov r5, r1
        bl i2cStart

        lsl r0, #24
        lsr r0, #25
        lsl r0, #1
        bl i2cOutb
        cmp r0, #0
         bne i2cWE
         
        lsl r0, r5, #24
        lsr r0, #24
        bl i2cOutb
        cmp r0, #0
         bne i2cWE

        cmp r3, #0
         ble i2cWN
i2cWL:  ldrb r0, [r2]
        bl i2cOutb
        cmp r0, #1
         beq i2cWN
        add r2, #1
        sub r3, #1
         bne i2cWL  
i2cWN:

        bl i2cStop

        mov r0, #0
        pop {r1, r2, r3, r4, r5, r6, r7}
        pop {r1}
        bx r1

i2cWE:  mov r0, #0
        sub r0, #1
        pop {r1, r2, r3, r4, r5, r6, r7}
        bx r1

@-------------------------------------------------------------------------------
@ i2cWriteRaw (r0=device, r1->buffer, r2=count)
@
.globl i2cWriteRawA
i2cWriteRawA:
        switchThumb
.globl i2cWriteRaw
.thumb_func

i2cWriteRaw:
        push {r1, r2, r3, r4, r5, r6, r7, lr}
        mov r4, r0
        
        ldr r7, =i2cBaseAddress
        ldrh r6, [r7, #i2cRegDR]
        mov r5, #4
        orr r6, r5
        strh r6, [r7, #i2cRegDR]

        ldrh r6, [r7, #i2cRegDR]
        mov r5, #8
        orr r6, r5
        strh r6, [r7, #i2cRegDR]

        ldrh r6, [r7, #i2cRegIN]
        lsr r6, #3
         bcc i2cWF

        ldrh r6, [r7, #i2cRegIN]
        lsr r6, #4
         bcc i2cWF

        mov r5, r1
        bl i2cStart

        lsl r0, #24
        lsr r0, #25
        lsl r0, #1
        bl i2cOutb
        cmp r0, #0
         bne i2cWF
         
        cmp r2, #0
         ble i2cWO
i2cWM:  ldrb r0, [r1]
        bl i2cOutb
        cmp r0, #1
         beq i2cWO
        add r1, #1
        sub r2, #1
         bne i2cWM  
i2cWO:

        bl i2cStop

        mov r0, #0
        pop {r1, r2, r3, r4, r5, r6, r7}
        pop {r1}
        bx r1

i2cWF:  mov r0, #0
        sub r0, #1
        pop {r1, r2, r3, r4, r5, r6, r7}
        bx r1
        
        .arm
        .ltorg
