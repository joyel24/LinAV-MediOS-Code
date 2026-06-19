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
@ u32 masControlWrite(u32 val)
@
@ u32 masReadData()

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
@ masWriteData()
@   
.globl masWriteDataA
masWriteDataA:
        switchThumb
.globl masWriteData
.thumb_func

masWriteData:
    push {r4, r5}
    mov r5, #0
    mov r4, r0              @ r4->data
    ldr r3, =0x30588
    ldrh r2, [r3, #0]
    lsr r2, #5
     bcs mwdto              @ Not ready to xfer data
    cmp r1, #0
     beq mwdto
mwdlp:

    ldrb r0, [r4, r5]       @ Get a byte of data...
    lsl r0, #24
    lsr r0, #16
    cmp r0, #0
     beq dne1
    strh r0, [r3, #0]       @ Set 1 bits
dne1:
    mov r2, #0xff
    lsl r2, #8
    eor r0, r2
    cmp r0, #0
     beq dne2
    strh r0, [r3, #4]       @ Set 0 bits
dne2:
    mov r0, #1
    lsl r0, #15
    strh r0, [r3, #2]       @ Try to latch data (Raise PR)
    mov r2, #10
mwdw:
    sub r2, #1
     bne mwdw

    ldrh r2, [r3, #2]       @ See if it worked...   (Read RTR)
    lsr r2, #15
     bcs mwdok
mwdw2:
    ldrh r2, [r3, #2]
    lsr r2, #15
     bcc mwdw2
     
mwdok:
    strh r0, [r3, #6]       @ Clear latch (Low PR)
    
    add r5, #1              @ Inc count
    sub r1, #1              @ Done another byte...
    ldrh r0, [r3, #0]
    lsr r0, #5
     bcs mwdto              @ EOD set! Quit...
    cmp r1, #0
     bne mwdlp

mwdto:
    mov r0, r5
    pop {r4, r5}
    bx lr

@ ------------------------------------------------------------------------------
@ masReadData(u32 buff, u32 maxdata)
@   
.globl masReadDataA
masReadDataA:
        switchThumb
.globl masReadData
.thumb_func

masReadData:
    push {r4, r5, r6}
    mov r4, #0              @ num bytes read...
    mov r5, r0
    mov r6, r1
    mov r0, #1
    lsl r0, #15
    ldr r1, =0x30588
mrdl:
    strh r0, [r1, #2]       @ Try to latch data
    ldrh r2, [r1, #2]       @ See if it worked...
    lsr r2, #15
     bcc mrdok
    mov r3, #3
mrdw:
    sub r3, #1
     beq mrdto              @ Timed out
    ldrh r3, [r1, #2]
    lsr r2, #15
     bcs mrdw
mrdok:
    nop
    nop
    nop
    nop
    nop
    ldrh r3, [r1, #0]       @ Get some data
    lsr r3, #8
    strb r3, [r5, r4]       @ Store the data...
    strh r0, [r1, #6]       @ Clear the latch...
    add r4, #1
    sub r6, #1
    cmp r6, #0
     bne mrdl

mrdto:
    strh r0, [r1, #6]       @ Clear the latch...    
    mov r0, r4
    pop {r4, r5, r6}
    bx lr

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

        ldr r0, =0x40000
masD1:  sub r0, #1
         bne masD1
        ldr r1, =0x3058a
        mov r0, #1
        strh r0, [r1]
        ldr r0, =0x40000
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
@ masControlRead()
@   
.globl masControlReadA
masControlReadA:
        switchThumb
.globl masControlRead
.thumb_func

masControlRead:
        push {lr}
        mov r3, r0
        bl i2cStart

        mov r0, #MAS_DEV_WRITE
        bl i2cOutb
        cmp r0, #0
         bne mcrE

        mov r0, #MAS_CONTROL
        bl i2cOutb
        cmp r0, #0
         bne mcrE
        
        bl i2cStart
         
        mov r0, #MAS_DEV_READ
        bl i2cOutb
        cmp r0, #0
         bne mcrE

        bl i2cInb
        mov r3, r0
        bl i2cAck

        bl i2cInb
        lsl r3, #24
        lsr r3, #16
        lsl r0, #24
        lsr r0, #24
        orr r3, r0
        bl i2cAckEnd

        bl i2cStop
    
        mov r0, r3
        pop {r1}
        bx r1
mcrE:
        mov r0, #0
        sub r0, #1
        pop {r1}
        bx r1
        
@ ------------------------------------------------------------------------------
@ masControlWrite(u32 data)
@   
.globl masControlWriteA
masControlWriteA:
        switchThumb
.globl masControlWrite
.thumb_func

masControlWrite:
        push {lr}
        mov r3, r0
        bl i2cStart
    
        mov r0, #MAS_DEV_WRITE
        bl i2cOutb
        cmp r0, #0
         bne mcwE

        mov r0, #MAS_CONTROL
        bl i2cOutb
        cmp r0, #0
         bne mcwE
        
        mov r0, r3
        lsr r0, #8
        bl i2cOutb
        cmp r0, #0
         bne mcwE

        mov r0, r3
        bl i2cOutb
        cmp r0, #0
         bne mcwE

        bl i2cStop
    
        mov r0, #0
        pop {r1}
        bx r1
mcwE:
        mov r0, #0
        sub r0, #1
        pop {r1}
        bx r1

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
        strb r0, [r4, #3]
        bl i2cAck
        
        bl i2cInb
        strb r0, [r4, #2]
        bl i2cAck
    
        bl i2cInb
        strb r0, [r4, #1]
        bl i2cAck

        bl i2cInb
        strb r0, [r4]
        add r4, #4

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
        ldrb r0, [r4, #3]
        bl i2cOutb

        ldrb r0, [r4, #2]
        bl i2cOutb

        ldrb r0, [r4, #1]
        bl i2cOutb

        ldrb r0, [r4]
        add r4, #4
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
        strb r0, [r4, #3]
        bl i2cAck
        
        bl i2cInb
        strb r0, [r4, #2]
        bl i2cAck
    
        bl i2cInb
        strb r0, [r4, #1]
        bl i2cAck

        bl i2cInb
        strb r0, [r4]
        add r4, #4

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
        ldrb r0, [r4, #3]
        bl i2cOutb

        ldrb r0, [r4, #2]
        bl i2cOutb

        ldrb r0, [r4, #1]
        bl i2cOutb

        ldrb r0, [r4]
        add r4, #4
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
        
        bl i2cInb
        lsl r0, #8
        mov r2, r0
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
