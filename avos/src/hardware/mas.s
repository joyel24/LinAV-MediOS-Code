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
@ u32 masWriteReg(u32 reg, u32 val)
@ u32 masReadCodecReg(u32 reg)
@ u32 masWriteCodecReg(u32 reg, u32 val)
@ u32 masReadD0(u32 addr, u32 buff, u32 size)
@ u32 masReadD1(u32 addr, u32 buff, u32 size)
@ u32 masWriteD0(u32 addr, u32 buff, u32 size)
@ u32 masWriteD1(u32 addr, u32 buff, u32 size)
@

.macro switchThumb
        .arm
        add ip, pc, #1
        bx ip
        .thumb
.endm

.ifndef masInc
masInc = 1

        MAS_DEV_WRITE       =   0x3c
        MAS_DEV_READ        =   0x3d
        
        MAS_DATA_WRITE      =   0x68
        MAS_DATA_READ       =   0x69
        MAS_CONTROL         =   0x6a
        MAS_CODEC_WRITE     =   0x6c
        MAS_CODEC_READ      =   0x6d

        MAS_CMD_READ_IC_VER =   0x70
        MAS_CMD_READ_REG    =   0xa0
        MAS_CMD_WRITE_REG   =   0xb0
        MAS_CMD_READ_D0     =   0xc0
        MAS_CMD_READ_D1     =   0xd0
        MAS_CMD_WRITE_D0    =   0xe0
        MAS_CMD_WRITE_D1    =   0xf0

.text

@ ------------------------------------------------------------------------------
@ masReset()
@   
.globl masResetA
masResetA:
        switchThumb
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
.globl masGetVersionA
masGetVersionA:
        switchThumb
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

        .ltorg
        
@ ------------------------------------------------------------------------------
@ masReadReg(u32 reg)
@   
.globl masReadRegA
masReadRegA:
        switchThumb
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

@ ------------------------------------------------------------------------------
@ masWriteReg(u32 reg, u32 data)
@   
.globl masWriteRegA
masWriteRegA:
        switchThumb
.globl masWriteReg
.thumb_func

masWriteReg:
        push {lr}
        mov r3, r0
        mov r2, r1
        bl i2cStart
    
        mov r0, #MAS_DEV_WRITE
        bl i2cOutb
        cmp r0, #0
         bne mwrE

        mov r0, #MAS_DATA_WRITE               @ 68
        bl i2cOutb
        cmp r0, #0
         bne mwrE
        
        mov r1, r3
        lsr r1, #4
        mov r0, #MAS_CMD_WRITE_REG
        orr r0, r1
        bl i2cOutb
        cmp r0, #0
         bne mwrE

        mov r0, r3
        lsl r0, #28
        lsr r0, #24
        mov r1, r2
        lsr r1, #16
        orr r0, r1
        bl i2cOutb
        cmp r0, #0
         bne mwrE
  
        mov r0, r2
        lsl r0, #16
        lsr r0, #24
        bl i2cOutb
        cmp r0, #0
         bne mwrE

        mov r0, r2
        lsl r0, #24
        lsr r0, #24
        bl i2cOutb
        cmp r0, #0
         bne mwrE

        bl i2cStop
    
        mov r0, #0
        pop {r1}
        bx r1
mwrE:
        mov r0, #0
        sub r0, #1
        pop {r1}
        bx r1

@ ------------------------------------------------------------------------------
@ masReadD0(u32 addr, u32 buffer, u32 size)
@   
.globl masReadD0A
masReadD0A:
        switchThumb
.globl masReadD0
.thumb_func

masReadD0:
        push {r4, r5, lr}
        mov r3, r0
        mov r4, r1
        mov r5, r2
        bl i2cStart
    
        mov r0, #MAS_DEV_WRITE
        bl i2cOutb
        cmp r0, #0
         bne mrd0E

        mov r0, #MAS_DATA_WRITE               @ 68
        bl i2cOutb
        cmp r0, #0
         bne mrd0E

        mov r0, #MAS_CMD_READ_D0
        bl i2cOutb
        cmp r0, #0
         bne mrd0E

        mov r0, #0
        bl i2cOutb
        cmp r0, #0
         bne mrd0E
         
        mov r0, r5
        lsr r0, #8
        bl i2cOutb
        cmp r0, #0
         bne mrd0E

        mov r0, r5
        bl i2cOutb
        cmp r0, #0
         bne mrd0E

        mov r0, r3
        lsr r0, #8
        bl i2cOutb
        cmp r0, #0
         bne mrd0E

        mov r0, r3
        bl i2cOutb
        cmp r0, #0
         bne mrd0E

        bl i2cStop
        
        bl i2cStart
        mov r0, #MAS_DEV_WRITE
        bl i2cOutb
        cmp r0, #0
         bne mrd0E

        mov r0, #MAS_DATA_READ                @ 69
        bl i2cOutb
        cmp r0, #0
         bne mrd0E
         
        bl i2cStart
        mov r0, #MAS_DEV_READ
        bl i2cOutb
        cmp r0, #0
         bne mrd0E
        
         
        @ r4 -> buffer
        @ r5 = n
mrd0l:
        bl i2cInb
        strb r0, [r4]
        add r4, #1
        bl i2cAck
        
        bl i2cInb
        strb r0, [r4]
        add r4, #1
        bl i2cAck
    
        bl i2cInb
        strb r0, [r4]
        add r4, #1
        bl i2cAck

        bl i2cInb
        strb r0, [r4]
        add r4, #1

        sub r5, #1
         beq mrd0d
        bl i2cAck
        b mrd0l
mrd0d:
        bl i2cAckEnd

        bl i2cStop
        mov r0, #0
        pop {r4, r5}
        pop {r1}
        bx r1
mrd0E:
        mov r0, #0
        sub r0, #1
        pop {r4, r5}
        pop {r1}
        bx r1

@ ------------------------------------------------------------------------------
@ masWriteD0(u32 addr, u32 buffer, u32 size)
@   
.globl masWriteD0A
masWriteD0A:
        switchThumb
.globl masWriteD0
.thumb_func

masWriteD0:
        push {r4, r5, lr}
        mov r3, r0
        mov r4, r1
        mov r5, r2
        bl i2cStart
    
        mov r0, #MAS_DEV_WRITE
        bl i2cOutb
        cmp r0, #0
         bne mwd0E

        mov r0, #MAS_DATA_WRITE               @ 68
        bl i2cOutb
        cmp r0, #0
         bne mwd0E

        mov r0, #MAS_CMD_WRITE_D0
        bl i2cOutb
        cmp r0, #0
         bne mwd0E

        mov r0, #0
        bl i2cOutb
        cmp r0, #0
         bne mwd0E
         
        mov r0, r5
        lsr r0, #8
        bl i2cOutb
        cmp r0, #0
         bne mwd0E

        mov r0, r5
        bl i2cOutb
        cmp r0, #0
         bne mwd0E

        mov r0, r3
        lsr r0, #8
        bl i2cOutb
        cmp r0, #0
         bne mwd0E

        mov r0, r3
        bl i2cOutb
        cmp r0, #0
         bne mwd0E
         
        @ r4 -> buffer
        @ r5 = n
mwd0l:
        ldrb r0, [r4]
        add r4, #1
        bl i2cOutb

        ldrb r0, [r4]
        add r4, #1
        bl i2cOutb

        ldrb r0, [r4]
        add r4, #1
        bl i2cOutb

        ldrb r0, [r4]
        add r4, #1
        bl i2cOutb
        
        sub r5, #1
        bne mwd0l

        bl i2cStop
        mov r0, #0
        pop {r4, r5}
        pop {r1}
        bx r1
mwd0E:
        mov r0, #0
        sub r0, #1
        pop {r4, r5}
        pop {r1}
        bx r1
        
@ ------------------------------------------------------------------------------
@ masReadD1(u32 addr, u32 buffer, u32 size)
@   
.globl masReadD1A
masReadD1A:
        switchThumb
.globl masReadD1
.thumb_func

masReadD1:
        push {r4, r5, lr}
        mov r3, r0
        mov r4, r1
        mov r5, r2
        bl i2cStart
    
        mov r0, #MAS_DEV_WRITE
        bl i2cOutb
        cmp r0, #0
         bne mrd1E

        mov r0, #MAS_DATA_WRITE               @ 68
        bl i2cOutb
        cmp r0, #0
         bne mrd1E

        mov r0, #MAS_CMD_READ_D1
        bl i2cOutb
        cmp r0, #0
         bne mrd1E

        mov r0, #0
        bl i2cOutb
        cmp r0, #0
         bne mrd1E
         
        mov r0, r5
        lsr r0, #8
        bl i2cOutb
        cmp r0, #0
         bne mrd1E

        mov r0, r5
        bl i2cOutb
        cmp r0, #0
         bne mrd1E

        mov r0, r3
        lsr r0, #8
        bl i2cOutb
        cmp r0, #0
         bne mrd1E

        mov r0, r3
        bl i2cOutb
        cmp r0, #0
         bne mrd1E

        bl i2cStop
        
        bl i2cStart
        mov r0, #MAS_DEV_WRITE
        bl i2cOutb
        cmp r0, #0
         bne mrd1E

        mov r0, #MAS_DATA_READ                @ 69
        bl i2cOutb
        cmp r0, #0
         bne mrd1E
         
        bl i2cStart
        mov r0, #MAS_DEV_READ
        bl i2cOutb
        cmp r0, #0
         bne mrd1E
        
         
        @ r4 -> buffer
        @ r5 = n
mrd1l:
        bl i2cInb
        strb r0, [r4]
        add r4, #1
        bl i2cAck
        
        bl i2cInb
        strb r0, [r4]
        add r4, #1
        bl i2cAck
    
        bl i2cInb
        strb r0, [r4]
        add r4, #1
        bl i2cAck

        bl i2cInb
        strb r0, [r4]
        add r4, #1

        sub r5, #1
         beq mrd1d
        bl i2cAck
        b mrd1l
mrd1d:
        bl i2cAckEnd

        bl i2cStop
        mov r0, #0
        pop {r4, r5}
        pop {r1}
        bx r1
mrd1E:
        mov r0, #0
        sub r0, #1
        pop {r4, r5}
        pop {r1}
        bx r1

@ ------------------------------------------------------------------------------
@ masWriteD1(u32 addr, u32 buffer, u32 size)
@   
.globl masWriteD1A
masWriteD1A:
        switchThumb
.globl masWriteD1
.thumb_func

masWriteD1:
        push {r4, r5, lr}
        mov r3, r0
        mov r4, r1
        mov r5, r2
        bl i2cStart
    
        mov r0, #MAS_DEV_WRITE
        bl i2cOutb
        cmp r0, #0
         bne mwd1E

        mov r0, #MAS_DATA_WRITE               @ 68
        bl i2cOutb
        cmp r0, #0
         bne mwd1E

        mov r0, #MAS_CMD_WRITE_D0
        bl i2cOutb
        cmp r0, #0
         bne mwd1E

        mov r0, #0
        bl i2cOutb
        cmp r0, #0
         bne mwd1E
         
        mov r0, r5
        lsr r0, #8
        bl i2cOutb
        cmp r0, #0
         bne mwd1E

        mov r0, r5
        bl i2cOutb
        cmp r0, #0
         bne mwd1E

        mov r0, r3
        lsr r0, #8
        bl i2cOutb
        cmp r0, #0
         bne mwd1E

        mov r0, r3
        bl i2cOutb
        cmp r0, #0
         bne mwd1E
         
        @ r4 -> buffer
        @ r5 = n
mwd1l:
        ldrb r0, [r4]
        add r4, #1
        bl i2cOutb

        ldrb r0, [r4]
        add r4, #1
        bl i2cOutb

        ldrb r0, [r4]
        add r4, #1
        bl i2cOutb

        ldrb r0, [r4]
        add r4, #1
        bl i2cOutb
        
        sub r5, #1
        bne mwd1l

        bl i2cStop
        mov r0, #0
        pop {r4, r5}
        pop {r1}
        bx r1
mwd1E:
        mov r0, #0
        sub r0, #1
        pop {r4, r5}
        pop {r1}
        bx r1
        
@ ------------------------------------------------------------------------------
@ masReadCodecReg(u32 reg)
@   
.globl masReadCodecRegA
masReadCodecRegA:
        switchThumb
.globl masReadCodecReg
.thumb_func

masReadCodecReg:
        push {lr}
        mov r3, r0
        bl i2cStart
    
        mov r0, #MAS_DEV_WRITE
        bl i2cOutb
        cmp r0, #0
         bne mrcE

        mov r0, #MAS_CODEC_WRITE
        bl i2cOutb
        cmp r0, #0
         bne mrcE
        
        mov r0, r3
        lsr r0, #8
        bl i2cOutb
        cmp r0, #0
         bne mrcE

        mov r0, r3
        bl i2cOutb
        cmp r0, #0
         bne mrcE
         
        bl i2cStop
        
        bl i2cStart
        mov r0, #MAS_DEV_WRITE
        bl i2cOutb
        cmp r0, #0
         bne mrcE

        mov r0, #MAS_CODEC_READ
        bl i2cOutb
        cmp r0, #0
         bne mrcE
         
        bl i2cStart
        mov r0, #MAS_DEV_READ
        bl i2cOutb
        cmp r0, #0
         bne mrcE
        
        mov r2, #0    
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
mrcE:
        mov r0, #0
        sub r0, #1
        pop {r1}
        bx r1

        
@ ------------------------------------------------------------------------------
@ masWriteCodecReg(u32 reg, u32 val)
@   
.globl masWriteCodecRegA
masWriteCodecRegA:
        switchThumb
.globl masWriteCodecReg
.thumb_func

masWriteCodecReg:
        push {lr}
        mov r3, r0
        mov r2, r1
        bl i2cStart
    
        mov r0, #MAS_DEV_WRITE
        bl i2cOutb
        cmp r0, #0
         bne mwcE

        mov r0, #MAS_CODEC_WRITE
        bl i2cOutb
        cmp r0, #0
         bne mwcE
        
        mov r0, r3
        lsr r0, #8
        bl i2cOutb
        cmp r0, #0
         bne mwcE

        mov r0, r3
        bl i2cOutb
        cmp r0, #0
         bne mwcE
         
        mov r0, r2
        lsr r0, #8
        bl i2cOutb
        cmp r0, #0
         bne mwcE

        mov r0, r2
        bl i2cOutb
        cmp r0, #0
         bne mwcE

        bl i2cStop
    
        mov r0, #0
        pop {r1}
        bx r1
mwcE:
        mov r0, #0
        sub r0, #1
        pop {r1}
        bx r1
        
        .arm
        .ltorg

.endif
