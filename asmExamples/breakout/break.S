@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@
@ Date:     24/01/2004
@ Author:   By DoggerMoore
@
@

.include "../includes/common.S"
.include "../includes/av300.S"

.text
        add r0, pc, #1
        bx r0               @ switch to thumb

        .thumb

        callThumb r1, lcdInitT
        
        ldr r3, =lcdVideoBuffer2
        ldr r4, =sprite
        ldr r5, =(320*240)
lcop:   mov r0, #0
        mov r1, #0
        mov r2, #0
        ldrb r0, [r4]
        ldrb r1, [r4, #1]
        ldrb r2, [r4, #2]
        add r4, #3
        callThumb r6, colorRGB2YCbCrT
        callThumb r6, colorPackYCbCrT

        str r0, [r3]
        
        add r3, #4
        sub r5, #1
        bne lcop   
        
        callThumb2 r3, osdSetBackConfigT, 0x14, 0x03
        callThumb2 r3, osdSetMainConfigT, 0x14, 0xd9      

        callThumb1 r3, osdSetCursorConfigT, 0x0201
        callThumb2 r3, osdSetCursorSizeT, 64, 8
        callThumb2 r3, osdSetCursorPositionT, 160, 100
        
@ Initialise the ball on the 2nd bitmap plane...
        
        callThumb2 r4, osdSetMain2ConfigT, 1, 0xd9
        callThumb2 r4, osdSetMain2SizeT, 16, 8
        callThumb2 r4, osdSetMain2PositionT, 160, 120
        callThumb1 r4, osdSetMain2OffsetT, lcdVideoBuffer3

        ldr r0, =theBall
        ldr r1, =lcdVideoBuffer3
        mov r2, #8
lcpyy:  mov r3, #8
lcpyx:  ldrh r4, [r0]
        strh r4, [r1]
        add r0, #2
        add r1, #2
        sub r3, #1
         bne lcpyx
        add r4, #1
        add r1, #16
        sub r2, #1
         bne lcpyy

        
initGame:       
        ldr r0, =oposx
        mov r1, #160
        str r1, [r0]
        mov r1, #120
        str r1, [r0, #4]
        mov r1, #160
        str r1, [r0, #8]
        mov r1, #120
        str r1, [r0, #12]
        mov r1, #1
        str r1, [r0, #16]
        str r1, [r0, #20]

        callThumb r1, lcdClsT

@ Init the brickmap, then draw the bricks...
        ldr r5, =brickMap
        ldr r6, =(10*12)
        mov r7, #1                  @ There is a brick there!
lopibm: strb r7, [r5]
        add r5, #1
        sub r6, #1
        cmp r6, #0
         bne lopibm

        @ Now draw the map on screen...
        mov r1, #0
        ldr r2, =brickMap
lbmy:   mov r0, #0
lbmx:   mov r2, #1
        callThumb r7, drawBrick
        add r0, #1
        cmp r0, #10
         blt lbmx
        add r1, #1
        cmp r1, #12
         blt lbmy
         
        
        ldr r0, =paddlePos
        ldr r0, [r0]
        lsl r0, #1
        mov r1, #230
        add r0, #0x14
        add r1, #0x12
        callThumb r3, osdSetCursorPositionT
loopb:


@ ---------------------------------------@ Draw the ball
        ldr r0, posx
        ldr r1, posy
        lsl r0, #1
        add r0, #0x14
        add r1, #0x12
        callThumb r4, osdSetMain2PositionT
        
        ldr r2, =0x4000
del1:   sub r2, #1
        bpl del1

        callThumb r1, buttonsGetStatusT
        ldr r1, =0x777
        eor r1, r0
        lsr r1, #10
        add r1, #1
                                        @ r1 = 1 or 2 for hard.
        lsl r1, #1
                                        
        ldr r3, =paddlePos
        ldr r4, [r3]
        mov r2, #2
        tst r0, r2
         bne noLt
        sub r4, r1
        cmp r4, #0
         bgt okSave 
        mov r4, #0
noLt:   mov r2, #0x10
        tst r0, r2
         bne noRt
        add r4, r1
        ldr r2, =(320-32)               @ 32 = paddleWidth for now.
        cmp r4, r2
         blt okSave
        mov r4, r2
noRt:   
okSave: ldr r2, [r3]
        cmp r2, r4
         beq noChange
        
        @ changed! save it, and redraw...
        str r4, [r3]

        mov r0, r4
        lsl r0, #1
        mov r1, #230
        add r0, #0x14
        add r1, #0x12
        callThumb r3, osdSetCursorPositionT

noChange:


@ ---------------------------------------@ Move the ball        
        ldr r0, posx
        ldr r1, =oposx
        str r0, [r1]
        ldr r0, posy
        ldr r1, =oposy
        str r0, [r1]

        ldr r0, posx
        ldr r1, posdx
        add r0, r0, r1
        cmp r0, #0
         blt oov1
        ldr r1, =304
        cmp r0, r1
        bge oov1
        ldr r1, =posx
        str r0, [r1, #0]
        b noov1
oov1:   ldr r1, posdx
        mov r0, #0
        sub r0, r1
        ldr r1, =posdx
        str r0, [r1, #0]    @ negate dx
noov1:
        
        ldr r0, posy
        ldr r1, posdy
        add r0, r0, r1
        cmp r0, #0
         blt oov3
        cmp r0, #223
        bge oov2
        ldr r1, =posy
        str r0, [r1, #0]
        b noov2
oov2:
        @ Ball has gone down onto paddle line...
        @ Is paddle in the right place?
        ldr r1, posx
        ldr r3, paddlePos
        mov r2, r1
        add r2, #8
        mov r4, r3
        add r4, #32
                        @ ball goes from r1 to r2
                        @ paddle goes from r3 to r4
        cmp r1, r4
        bgt lostBall    @ ball sx > paddle ex
        cmp r2, r3
        blt lostBall    @ ball ex < paddle sx

        @ Change dx as well depending on position on paddle...
        @         r1|   |r2
        @ r3-------- -------- -------- --------r4
        
        @ (r1 + 4) - (r3 + 16)
        @ r1 - r3 - 12
        
        sub r1, r3      @ r1 = offset into paddle
        sub r1, #12
        asr r1, #3      @ scale down a bit?
        
        ldr r0, =posdx
        str r1, [r0]    @ set dx.
        
oov3:   ldr r1, posdy
        mov r0, #0
        sub r0, r1
        ldr r1, =posdy
        str r0, [r1, #0]    @ negate dy        
noov2:

        @ Now check for colision with any bricks... (Harder)
        
        @ TLC of ball
        ldr r0, posx
        ldr r1, posy
        callThumb r2, checkBrick

        @ TRC of ball
        ldr r0, posx
        ldr r1, posy
        add r0, #8
        callThumb r2, checkBrick

        @ BLC of ball
        ldr r0, posx
        ldr r1, posy
        add r1, #8
        callThumb r2, checkBrick

        @ BRC of ball
        ldr r0, posx
        ldr r1, posy
        add r0, #8
        add r1, #8
        callThumb r2, checkBrick

        b loopb

lostBall:
        b initGame
        

@ ------------------------------------------------------------------------------
checkBrick:
        push {lr}
        push {r1, r2, r3, r4, r5}
        lsr r0, #5              @ /32
        lsr r1, #3              @ /8
        mov r4, r0
        mov r5, r1
        mov r2, #10
        mul r1, r2
        add r1, r0              @ r1->brickPos in array
        ldr r2, =brickMap
        mov r0, #0
        ldr r3, =(10*12) 
        cmp r1, r3
         bge uBricks
        ldrb r0, [r2, r1]
        cmp r0, #0
         beq uBricks
        mov r3, #0
        strb r3, [r2, r1]       @ Erase the brick!
        mov r2, #0
        mov r0, r4
        mov r1, r5
        callThumb r4, drawBrick

        @ Make it rebound off brick as well...
        
        @ Work out delta x and delta y in the brick map plane...
        
        ldr r0, posx
        ldr r1, oposx
        lsr r0, #5
        lsr r1, #5
        cmp r0, r1
        beq norebx
                                @ Negate dx
        ldr r0, posdx
        mov r1, #0
        sub r1, r0
        ldr r0, =posdx
        str r1, [r0]
        
norebx:
        ldr r0, posy
        ldr r1, oposy
        lsr r0, #3
        lsr r1, #3
        cmp r0, r1
        beq noreby
                                @ Negate dy 
        ldr r0, posdy
        mov r1, #0
        sub r1, r0
        ldr r0, =posdy
        str r1, [r0]

noreby:
        ldr r0, oposx
        ldr r1, =posx
        str r0, [r1]        
        ldr r0, oposy
        ldr r1, =posy
        str r0, [r1]        


        mov r0, #1
uBricks:pop {r1, r2, r3, r4, r5}
        pop {r4}
        bx r4

        
@ ------------------------------------------------------------------------------
drawBrick:
        push {lr}
        push {r0, r1, r2, r3, r4, r5}
        
        mov r4, #0
        cmp r2, #1
         bne nob
        ldr r4, =0x0404
nob:
        mov r3, #32
        mul r0, r3
        mov r3, #8
        mul r1, r3
                            @ r0, r1 -> TLC of brick
        add r0, #1
        add r1, #1
        mov r2, #30
        mov r3, #6

        callThumb r5, lcdRectfT
        
        pop {r0, r1, r2, r3, r4, r5}
        pop {r4}
        bx r4

        .align 4        

        .ltorg      

oposx:  .word 160
oposy:  .word 160
posx:   .word 160
posy:   .word 120
posdx:  .word 1
posdy:  .word 1

paddlePos:  .word 146

        .align 4
theBall:
.hword 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 
.hword 0x0000, 0x0000, 0x0404, 0x0404, 0x0404, 0x0404, 0x0000, 0x0000 
.hword 0x0000, 0x0404, 0x0404, 0xffff, 0xffff, 0x0404, 0x0404, 0x0000 
.hword 0x0000, 0x0404, 0xffff, 0xffff, 0xffff, 0xffff, 0x0404, 0x0000 
.hword 0x0000, 0x0404, 0xffff, 0xffff, 0xffff, 0xffff, 0x0404, 0x0000 
.hword 0x0000, 0x0404, 0x0404, 0xffff, 0xffff, 0x0404, 0x0404, 0x0000 
.hword 0x0000, 0x0000, 0x0404, 0x0404, 0x0404, 0x0404, 0x0000, 0x0000 
.hword 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000

        .align 4
brickMap:
.space 80, 0

@ This stores the status of bricks. Are they on or off?
@ Used for colision detection etc...
                
.include "../includes/lcd.S"
.include "../includes/buttons.S"
.include "../includes/color.S"

sprite:
.incbin     "image.rgb"
