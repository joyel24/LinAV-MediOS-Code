@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@
@ Date:     21/02/2004
@ Author:   By DoggerMoore
@
@ void masReset()
@ u32 masGetVersion()
@ u32 masReadReg(u32 reg)
@

.ifndef masInc
masInc = 1

        MAS_DEV_WRITE       =   0x3c
        MAS_DEV_READ        =   0x3d
        MAS_CONTROL         =   0x6a
        MAS_DATA_READ       =   0x69
        MAS_DATA_WRITE      =   0x68
        MAS_CMD_READ_IC_VER =   0x70
        MAS_CMD_READ_REG    =   0xa0

.text

@ ------------------------------------------------------------------------------
@ masReset()
@   
.globl masReset
.thumb_func

masReset:
        push {r0, r1}
        ldr r1, =0x3058e
        mov r0, #1
        strh r0, [r1]
        
        ldr r0, =0x20000
masD1:  sub r0, #1
         bne masD1
        ldr r1, =0x3058a
        mov r0, #1
        strh r0, [r1]
        ldr r0, =0x20000
masD2:  sub r0, #1
         bne masD2
        
        pop {r0, r1}
        bx lr


@ ------------------------------------------------------------------------------
@ masGetVersion()
@   
.globl masGetVersion
.thumb_func

masGetVersion:
        push {lr}
        bl i2cStart
    
        mov r0, #MAS_DEV_WRITE
        bl i2cOutb
        cmp r0, #0
         bne mgvE
    
        mov r0, #MAS_CONTROL                @ 6a
        bl i2cOutb
        cmp r0, #0
         bne mgvE
        
        mov r0, #0x8c
        bl i2cOutb
        cmp r0, #0
         bne mgvE
        
        mov r0, #0x00
        bl i2cOutb
        cmp r0, #0
         bne mgvE
        
        bl i2cStop
        
        ldr r0, =0x2000
mgvD1:  sub r0, #1
         bne mgvD1

        bl i2cStart
        mov r0, #MAS_DEV_WRITE
        bl i2cOutb
        cmp r0, #0
         bne mgvE

        mov r0, #MAS_DATA_WRITE               @ 68
        bl i2cOutb
        cmp r0, #0
         bne mgvE
         
        mov r0, #MAS_CMD_READ_IC_VER
        bl i2cOutb
        cmp r0, #0
         bne mgvE

        mov r0, #0x00
        bl i2cOutb
        cmp r0, #0
         bne mgvE
         
        bl i2cStop
        
        bl i2cStart
        mov r0, #MAS_DEV_WRITE
        bl i2cOutb
        cmp r0, #0
         bne mgvE

        mov r0, #MAS_DATA_READ                @ 69
        bl i2cOutb
        cmp r0, #0
         bne mgvE
         
        bl i2cStart
        mov r0, #MAS_DEV_READ
        bl i2cOutb
        cmp r0, #0
         bne mgvE
        
        mov r2, #0
        bl i2cInb
        lsl r0, #24
        orr r2, r0        
        bl i2cAck
        
        bl i2cInb
        lsl r0, #16
        orr r2, r0        
        bl i2cAck
    
        bl i2cInb
        lsl r0, #8
        orr r2, r0
        bl i2cAck

        bl i2cInb
        orr r2, r0        
        bl i2cAckEnd

        bl i2cStop
    
        mov r0, r2
        pop {r1}
        bx r1
mgvE:
        mov r0, #0
        sub r0, #1
        pop {r1}
        bx r1

        
@ ------------------------------------------------------------------------------
@ masReadReg(u32 reg)
@   
.globl masReadReg
.thumb_func

masReadReg:
        push {lr}
        mov r3, r0
        bl i2cStart
    
        mov r0, #MAS_DEV_WRITE
        bl i2cOutb
        cmp r0, #0
         bne mrrE

        mov r0, #MAS_DATA_WRITE               @ 68
        bl i2cOutb
        cmp r0, #0
         bne mrrE
        
        mov r1, r3
        lsr r1, #4
        mov r0, #MAS_CMD_READ_REG
        orr r0, r1
        bl i2cOutb
        cmp r0, #0
         bne mrrE

        mov r0, r3
        lsl r0, #28
        lsr r0, #24
        bl i2cOutb
        cmp r0, #0
         bne mrrE
         
        bl i2cStop
        
        bl i2cStart
        mov r0, #MAS_DEV_WRITE
        bl i2cOutb
        cmp r0, #0
         bne mrrE

        mov r0, #MAS_DATA_READ                @ 69
        bl i2cOutb
        cmp r0, #0
         bne mrrE
         
        bl i2cStart
        mov r0, #MAS_DEV_READ
        bl i2cOutb
        cmp r0, #0
         bne mrrE
        
        mov r2, #0
        bl i2cInb
        lsl r0, #24
        orr r2, r0        
        bl i2cAck
        
        bl i2cInb
        lsl r0, #16
        orr r2, r0        
        bl i2cAck
    
        bl i2cInb
        lsl r0, #8
        orr r2, r0
        bl i2cAck

        bl i2cInb
        orr r2, r0        
        bl i2cAckEnd

        bl i2cStop
    
        mov r0, r2
        pop {r1}
        bx r1
mrrE:
        mov r0, #0
        sub r0, #1
        pop {r1}
        bx r1
        

        .arm
        .ltorg

.endif
