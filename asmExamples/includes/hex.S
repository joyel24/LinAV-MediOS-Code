@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@ Hex - Useful routines for converting hex.
@
@
@ Date:     04/01/2004
@ Author:   By DoggerMoore
@
@ hex2T(r0->destnation, r1=value)
@ hex4T(r0->destnation, r1=value)
@ hex8T(r0->destnation, r1=value)
@

.ifndef hexInc
hexInc = 1

.text

        .thumb

@ hex2T  r0->dest, r1=value
@
hex2T:
        push {r1, r2, r3, r4}
        ldr r4, =hextable
        mov r2, #0x0f
        mov r3, r1              @ save for later
        lsr r1, r3, #4
        and r1, r2
        ldrb r1, [r4, r1]       @ get 1st byte
        strb r1, [r0, #0]       @ store it
        and r2, r3              @ r1 = LO
        ldrb r1, [r4, r2]       @ get 2nd byte
        strb r1, [r0, #1]       @ store it
        pop {r1, r2, r3, r4}
        bx lr

@ hex4T  r0->dest, r1=value (A.B.C.D)
@
hex4T:
        push {r1, r2, r3, r4}
        ldr r4, =hextable
        mov r2, #0x0f
        mov r3, r1              @ save for later
        lsr r1, r3, #12         @ r1 = A
        and r1, r2
        ldrb r1, [r4, r1]       @ get 1st byte
        strb r1, [r0, #0]       @ store it

        lsr r1, r3, #8          @ r1 = B
        and r1, r2
        ldrb r1, [r4, r1]       @ get 2nd byte
        strb r1, [r0, #1]       @ store it
    
        lsr r1, r3, #4          @ r1 = C
        and r1, r2
        ldrb r1, [r4, r1]       @ get 3rd byte
        strb r1, [r0, #2]       @ store it

        and r2, r3              @ r1 = D
        ldrb r1, [r4, r2]       @ get 4th byte
        strb r1, [r0, #3]       @ store it
        pop {r1, r2, r3, r4}
        bx lr

hextable:   .ascii "0123456789ABCDEF"
        .ltorg

@ hex8T  r0->dest, r1=value (A.B.C.D.E.F.G.H)
@
hex8T:
        push {r1, r2, r3, r4}
        ldr r4, =hextable
        mov r2, #0x0f
    
        mov r3, r1              @ save for later
    
        lsr r1, r3, #28         @ r1 = A
        and r1, r2
        ldrb r1, [r4, r1]       @ get 1st byte
        strb r1, [r0, #0]       @ store it
    
        lsr r1, r3, #24          @ r1 = B
        and r1, r2
        ldrb r1, [r4, r1]       @ get 2nd byte
        strb r1, [r0, #1]       @ store it

        lsr r1, r3, #20         @ r1 = C
        and r1, r2
        ldrb r1, [r4, r1]       @ get 3rd byte
        strb r1, [r0, #2]       @ store it

        lsr r1, r3, #16         @ r1 = D
        and r1, r2
        ldrb r1, [r4, r1]       @ get 4th byte
        strb r1, [r0, #3]       @ store it

        lsr r1, r3, #12         @ r1 = E
        and r1, r2
        ldrb r1, [r4, r1]       @ get 5th byte
        strb r1, [r0, #4]       @ store it

        lsr r1, r3, #8          @ r1 = F
        and r1, r2
        ldrb r1, [r4, r1]       @ get 6th byte
        strb r1, [r0, #5]       @ store it

        lsr r1, r3, #4          @ r1 = G
        and r1, r2
        ldrb r1, [r4, r1]       @ get 7th byte
        strb r1, [r0, #6]       @ store it
    
        and r2, r3              @ r1 = H
        ldrb r1, [r4, r2]       @ get 8th byte
        strb r1, [r0, #7]       @ store it
        pop {r1, r2, r3, r4}
        bx lr

        .ltorg
        .arm
        
.endif
