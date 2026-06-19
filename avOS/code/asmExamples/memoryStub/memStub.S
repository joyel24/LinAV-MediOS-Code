@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@ memoryStub
@
@ Date:     27/01/2004
@ Author:   By DoggerMoore
@
@

.include "../includes/common.S"
.include "../includes/av300.S"

.text

        add r0, pc, #1
        bx r0                   @ switch to thumb
        
        .thumb
        callThumb r0, lcdInitT

        ldr r6, =inbuff             @ r6->current pos in inbuff...
mloop:  callThumb r1, uartGetchT
        ldr r1, =(-1)
        cmp r0, r1
         beq mloop                  @ no input!
        strb r0, [r6]
        ldr r1, =(inbuffe-1)
        cmp r6, r1
         beq bufffull
        add r6, #1                  @ Move on a byte in buffer...
bufffull:
        lsl r0, #24
        lsr r0, #24
        cmp r0, #0x0d               @ CR?
         bne mloop
         
        
        mov r0, #0
        strb r0, [r6]               @ Make sure its terminated
        ldr r6, =inbuff             @ Reset ptr...
        
        mov r0, r6
        
        @@@ Now do what the command wants...
                                    @ rb00000000    Read byte from 00000000
                                    @ rh00000000    Read half from 00000000
                                    @ rw00000000    Read word from 00000000
        mov r0, #0
        mov r5, r6
        ldrb r0, [r5]
        cmp r0, #'r'
         bne notR
        add r5, #1
        ldrb r0, [r5]
        add r5, #1
        cmp r0, #'b'
         bne notrb
        mov r0, r5
        mov r1, #8
        callThumb r2, hexreadT      @ Get address

        mov r4, r0
        mov r1, r0
        ldr r0, =addressE
        callThumb r2, hex8T
        callThumb1 r2, uartOutsT, addressE
        mov r0, r4
        
        mov r1, #0
        ldrb r1, [r0]
        ldr r0, =hex2buffr
        callThumb r2, hex2T
        callThumb1 r2, uartOutsT, hex2buffr
        b mloop
notrb:  cmp r0, #'h'
         bne notrh
        mov r0, r5
        mov r1, #8
        callThumb r2, hexreadT
        mov r4, r0
        mov r1, r0
        ldr r0, =addressE
        callThumb r2, hex8T
        callThumb1 r2, uartOutsT, addressE
        mov r0, r4

        mov r1, #0
        ldrh r1, [r0]
        ldr r0, =hex4buffr
        callThumb r2, hex4T
        callThumb1 r2, uartOutsT, hex4buffr        
        b mloop
notrh:  cmp r0, #'w'
         bne notrw
        mov r0, r5
        mov r1, #8
        callThumb r2, hexreadT
        mov r4, r0
        mov r1, r0
        ldr r0, =addressE
        callThumb r2, hex8T
        callThumb1 r2, uartOutsT, addressE
        mov r0, r4

        ldr r1, [r0]
        ldr r0, =hex8buffr
        callThumb r2, hex8T
        callThumb1 r2, uartOutsT, hex8buffr
        b mloop
notrw:
        b mloop
notR:
                                    @ wb00000000 00 Write byte
                                    @ wh00000000 0000
                                    @ ww00000000 00000000
        add r5, #1
        ldrb r0, [r5]
        add r5, #1
        cmp r0, #'b'
         bne notwb
        mov r0, r5
        mov r1, #8
        callThumb r2, hexreadT
        mov r4, r0
        mov r1, r0
        ldr r0, =addressE
        callThumb r2, hex8T
        callThumb1 r2, uartOutsT, addressE
        mov r0, r4

        mov r4, r0                  @ r4 = address
        mov r0, r5
        add r0, #9
        mov r1, #2
        callThumb r2, hexreadT
        strb r0, [r4]               @ Do the write...
        mov r1, r0
        ldr r0, =hex2buffw
        callThumb r2, hex2T
        callThumb1 r2, uartOutsT, hex2buffw        
        b mloop
notwb:  cmp r0, #'h'
         bne notwh
        mov r0, r5
        mov r1, #8
        callThumb r2, hexreadT
        mov r4, r0
        mov r1, r0
        ldr r0, =addressE
        callThumb r2, hex8T
        callThumb1 r2, uartOutsT, addressE
        mov r0, r4

        mov r4, r0                  @ r4 = address
        mov r0, r5
        add r0, #9
        mov r1, #4
        callThumb r2, hexreadT
        strh r0, [r4]               @ Do the write...
        mov r1, r0
        ldr r0, =hex4buffw
        callThumb r2, hex4T
        callThumb1 r2, uartOutsT, hex4buffw        
        b mloop
notwh:  cmp r0, #'w'
         bne notww
        mov r0, r5
        mov r1, #8
        callThumb r2, hexreadT
        mov r4, r0
        mov r1, r0
        ldr r0, =addressE
        callThumb r2, hex8T
        callThumb1 r2, uartOutsT, addressE
        mov r0, r4

        mov r4, r0                  @ r4 = address
        mov r0, r5
        add r0, #9
        mov r1, #8
        callThumb r2, hexreadT
        str r0, [r4]               @ Do the write...
        mov r1, r0
        ldr r0, =hex8buffw
        callThumb r2, hex8T
        callThumb1 r2, uartOutsT, hex8buffw
        b mloop
notww:                              
                                    
noend:
        b mloop

        .align
inbuff: .space 80, 0                @ Input buffer
inbuffe:.byte 0

addressE:    .asciz  "xxxxxxxx A\n"

hex2buffr:   .asciz  "xx r\n"
hex4buffr:   .asciz  "xxxx r\n"
hex8buffr:   .asciz  "xxxxxxxx r\n"

hex2buffw:   .asciz  "xx w\n"
hex4buffw:   .asciz  "xxxx w\n"
hex8buffw:   .asciz  "xxxxxxxx w\n"

        .align        

.include "../includes/uart.S"
.include "../includes/terminal.S"
.include "../includes/hex.S"
