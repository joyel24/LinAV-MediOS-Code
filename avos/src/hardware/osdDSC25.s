@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@ osd - (DSC25)
@
@ Date:     09/02/2004
@ Author:   By DoggerMoore
@
@ void osdSetComponentOffset (r0=component, r1=offset)
@ void osdSetComponentSize (r0=component, r1=width, r2=height)
@ void osdSetComponentPosition (r0=component, r1=x, r2=y)
@ void osdSetComponentConfig (r0=component, r1=config)
@ void osdSetComponentSourceWidth (r0=component, r1=width)
@ void osdSetCursor2Bitmap(r0=index, r1=data)
@
@ void osdSetPallette (r0=Y, r1=Cr, r2=Cb, r3=index)
@ void osdSet16CPallete (r0=bank0/1, r1=index, r2=value)
@
@ void osdSetBrightness (r0=0-255)
@ void osdSetContrast (r0=0-255)
@
@ void osdSetMainShift (r0=horizontal, r1=vertical)
@ void osdSetMainConfig (r0=config)
@ void osdSetBorderColor (r0=color)
@ void osdSetAltOffset (r0=offset)
@
@ TODO: 30680: {9, 12, 13, 14}
@       30682: {6, 7}
@       30684: {13}
@       30686: {14, 15}
@       3068a: {1-7}
@       306f4: {0-6}
@       306f6: {0-15}
@       306f8: {0-15}
@

.ifndef osdInc
osdInc = 1

.text
        .thumb

        OSD_VIDEO1          =   0
        OSD_VIDEO2          =   1
        OSD_BITMAP1         =   2
        OSD_BITMAP2         =   3
        OSD_CURSOR1         =   4
        OSD_CURSOR2         =   5

        OSD_MAIN_BORDER_RAMCLUT =   0x01
        OSD_MAIN_VIDEO_SCALEX   =   0x04
        OSD_MAIN_VIDEO_SCALEY   =   0x08
        OSD_MAIN_VIDEO_SCALEXM  =   0x10
        OSD_MAIN_SWAPRB         =   0x80
        
        OSD_COMPONENT_ENABLE    =   0x0001      @ 0000000000000001

@ VIDEO Configuration options
        
        OSD_VIDEO_HALFHEIGHT    =   0x0002      @ 00000010
        OSD_VIDEO_ZY0           =   0x0000      @ 00001100
        OSD_VIDEO_ZY1           =   0x0004
        OSD_VIDEO_ZY2           =   0x0008
        OSD_VIDEO_ZX0           =   0x0000      @ 00110000
        OSD_VIDEO_ZX1           =   0x0010
        OSD_VIDEO_ZX2           =   0x0020

@ BITMAP Configuration options

        OSD_BITMAP_HALFHEIGHT   =   0x0002      @ 0000000000000010
        OSD_BITMAP_MERGEBACK    =   0x0000      @ 0000000000000100
        OSD_BITMAP_0TRANS       =   0x0004

        OSD_BITMAP_A_SHIFT      =   3
        OSD_BITMAP_A0           =   0x0000      @ 0000000000111000
        OSD_BITMAP_A1           =   0x0008
        OSD_BITMAP_A2           =   0x0010
        OSD_BITMAP_A3           =   0x0018
        OSD_BITMAP_A4           =   0x0020
        OSD_BITMAP_A5           =   0x0028
        OSD_BITMAP_A6           =   0x0030
        OSD_BITMAP_A7           =   0x0038
        
        OSD_BITMAP_1BIT         =   0x0000      @ 0000000011000000
        OSD_BITMAP_2BIT         =   0x0040
        OSD_BITMAP_4BIT         =   0x0080
        OSD_BITMAP_8BIT         =   0x00c0
        
        OSD_BITMAP_ZY0          =   0x0000      @ 0000111100000000
        OSD_BITMAP_ZY1          =   0x0100
        OSD_BITMAP_ZY2          =   0x0200
        OSD_BITMAP_ZX0          =   0x0000
        OSD_BITMAP_ZX1          =   0x0400
        OSD_BITMAP_ZX2          =   0x0800

        OSD_BITMAP_RAMCLUT      =   0x1000      @ 0001000000000000

@ CURSOR1 Configuration options

        OSD_CURSOR1_RAMCLUT             =   0x0080
        OSD_CURSOR1_BORDERHEIGHT_SHIFT  =   1       @ 0-7
        OSD_CURSOR1_BORDERWIDTH_SHIFT   =   4       @ 0-7
        OSD_CURSOR1_COLOR_SHIFT         =   8       @ 0-255

@ CURSOR2 Configuration options

        OSD_CURSOR2_HALFHEIGHT          =   0x0002
        OSD_CURSOR2_RAMCLUT             =   0x0004
        
        OSD_CURSOR2_COLORBANK0          =   0x0000
        OSD_CURSOR2_COLORBANK1          =   0x0008   

        OSD_CURSOR2_ZY0                 =   0x0000      @ 0000111100000000
        OSD_CURSOR2_ZY1                 =   0x0100
        OSD_CURSOR2_ZY2                 =   0x0200
        OSD_CURSOR2_ZX0                 =   0x0000
        OSD_CURSOR2_ZX1                 =   0x0400
        OSD_CURSOR2_ZX2                 =   0x0800
        OSD_CURSOR2_FLASH_SPEED0        =   0x0000
        OSD_CURSOR2_FLASH_SPEED1        =   0x1000
        OSD_CURSOR2_FLASH_SPEED2        =   0x2000
        OSD_CURSOR2_FLASH_SPEED3        =   0x3000
        OSD_CURSOR2_FLASH_ENABLE        =   0x4000
        
        
@ ==============================================================================

osdLookupOffsetLO:  .word 0x30696
                    .word 0x30698
                    .word 0x3069c
                    .word 0x3069e
osdLookupOffsetHI:  .word 0x30694
                    .word 0x30695
                    .word 0x3069a
                    .word 0x3069b


@ ------------------------------------------------------------------------------
@ osdSetCursor2Bitmap(r0=index, r1=data)
@
.globl osdSetCursor2Bitmap
.thumb_func

osdSetCursor2Bitmap:
        push {r1, r2, lr}
        ldr r2, =0x306f2
        strh r1, [r2]           @ Setup data reg
        
        ldrh r1, [r2, #2]
        lsl r1, #24
        lsr r1, #24
        lsl r0, #8
        orr r1, r0
        mov r0, #0x80
        orr r1, r0
        strh r1, [r2, #2]       @ Set the data...
        pop {r1, r2, pc}

                    
@ ------------------------------------------------------------------------------
@ osdSetBorderColor(r0=color)
@
.globl osdSetBorderColor
.thumb_func

osdSetBorderColor:
        push {r1, r2, lr}
        ldr r2, =0x30680
        ldrh r1, [r2]
        lsr r1, #8
        lsl r1, #8
        orr r1, r0
        strh r1, [r2]
        pop {r1, r2, pc}

        
@ ------------------------------------------------------------------------------
@ osdSetMainConfig(r0=config)
@
.globl osdSetMainConfig
.thumb_func

osdSetMainConfig:
        push {r0, r1, r2, lr}
        ldr r2, =0x30680
        ldrh r1, [r2]
        lsl r1, #24
        lsr r1, #24
        lsl r0, #8
        orr r1, r0
        strh r1, [r2]
        pop {r0, r1, r2, pc}
        
                    
@ ------------------------------------------------------------------------------
@ osdSetMainShift(r0=horizontal, r1=vertical)
@
.globl osdSetMainShift
.thumb_func

osdSetMainShift:
        push {r2, lr}
        ldr r2, =0x306a0
        strh r0, [r2]
        strh r1, [r2, #2]
        pop {r2, pc}
                    

@ ------------------------------------------------------------------------------
@ osdSetPallette(r0=Y, r1=Cr, r2=Cb, r3=index)
@
.globl osdSetPallette
.thumb_func

osdSetPallette:
        push {r0, r1, r3, r4, r5, lr}
        mov r5, #0
        ldr r4, =0x306f4
osdWait:ldrh r5, [r4]
        cmp r5, #0
         bne osdWait
        lsl r1, #8
        orr r3, r1
        strh r3, [r4, #4]
osdWaiu:ldrh r5, [r4]
        cmp r5, #0
         bne osdWaiu
        lsl r0, #8
        orr r0, r2
        strh r0, [r4, #2]
        pop {r0, r1, r3, r4, r5, pc}

        
@ ------------------------------------------------------------------------------
@ osdSet16cPallette(r0=bank0/1, r1=index, r2=value)
@
.globl osdSet16CPallette
.thumb_func

osdSet16CPallette:
        push {r0, r1, r3, r4, r5, lr}
        lsl r0, #4
        mov r4, r1
        lsl r4, #31
        lsr r4, #31
        lsl r4, #3
        ldr r5, =0xff00
        lsr r5, r4
        lsl r2, r4
        lsr r1, #1
        lsl r1, #1
        add r1, r0
        ldr r0, =0x306d0
        ldrh r3, [r0, r1]
        and r3, r5
        orr r3, r2
        strh r3, [r0, r1]
        pop {r0, r1, r3, r4, r5, pc}

        
@ ------------------------------------------------------------------------------
@ osdSetAltOffset(r0=offset)
@
.globl osdSetAltOffset
.thumb_func

osdSetAltOffset:
        push {r0, r1, r2, r3, r4, lr}
        ldr r2, =0x3000000
        sub r1, r2
        lsr r1, #5
        ldr r2, =0x306fc
        strh r1, [r2, #2]
        lsr r1, #16
        strh r1, [r2]
        pop {r0, r1, r2, r3, r4, pc}
        

@ ------------------------------------------------------------------------------
@ osdSetComponentOffset(r0=component, r1=offset)
@       VIDEO1,VIDEO2,BITMAP1,BITMAP1
@
.globl osdSetComponentOffset
.thumb_func

osdSetComponentOffset:
        push {r0, r1, r2, r3, r4, lr}
        ldr r2, =0x3000000
        sub r1, r2
        lsr r1, #5

        ldr r2, =osdLookupOffsetLO
        lsl r0, #2
        ldr r2, [r2, r0]                @ r2 = hardware reg for offs LO
        strh r1, [r2]                   @ Done LO.
        lsr r1, #16

        ldr r2, =osdLookupOffsetHI
        ldr r2, [r2, r0]                @ r2 = hardware reg for offs HI
        mov r3, r2
        lsr r2, #1
        lsl r2, #1
        ldrh r0, [r2]                   @ r1 = old val
        lsl r3, #31
        lsr r3, #31                     @ 0/1
        lsl r3, #3                      @ 0/8
        ldr r4, =0xff00
        lsr r4, r3                      @ ff00 / 00ff
        and r0, r4
        lsl r1, r3                      @ 00vv / vv00
        orr r1, r0
        strh r1, [r2]
        pop {r0, r1, r2, r3, r4, pc}

        
@ ------------------------------------------------------------------------------
@ osdSetComponentSize r0=component, r1=width, r2=height
@   VIDEO1,VIDEO2,BITMAP1,BITMAP2,CURSOR1
@   Note that width should be *2 usually
@
.globl osdSetComponentSize
.thumb_func

osdSetComponentSize:
        push {r0, r3, lr}
        lsl r0, #3                      @ component *8
        ldr r3, =0x306a8
        add r3, r0
        strh r1, [r3]
        strh r2, [r3, #2]
        pop {r0, r3, pc}


@ ------------------------------------------------------------------------------
@ osdSetComponentPosition r0=component, r1=x, r2=y
@   VIDEO1,VIDEO2,BITMAP1,BITMAP2,CURSOR1,CURSOR2
@   Note that x should be *2 usually
@
.globl osdSetComponentPosition
.thumb_func

osdSetComponentPosition:
        push {r0, r3, lr}
        lsl r0, #3                      @ component *8
        ldr r3, =0x306a4
        add r3, r0
        strh r1, [r3]
        strh r2, [r3, #2]
        pop {r0, r3, pc}


@ ------------------------------------------------------------------------------        
@ osdSetComponentSourceWidth r0=component, r1=width
@   VIDEO1,VIDEO2,BITMAP1,BITMAP2
@   Note that width should be *2 usually
@
.globl osdSetComponentSourceWidth
.thumb_func

osdSetComponentSourceWidth:
        push {r0, r2, lr}
        lsl r0, #1                      @ component *2
        ldr r2, =0x3068c
        strh r1, [r2, r0]
        pop {r0, r2, pc}


@ ------------------------------------------------------------------------------
@ osdSetComponentConfig r0=component, r1=config
@   VIDEO1,VIDEO2,BITMAP1,BITMAP2,CURSOR1,CURSOR2
@
.globl osdSetComponentConfig
.thumb_func

osdSetComponentConfig:
        push {r0, r2, r3, lr}
        cmp r0, #OSD_VIDEO1
         bne osdNV1
        ldr r2, =0x30682
        ldrh r3, [r2]
        lsr r3, #8
        lsl r3, #8
        orr r1, r3
        strh r1, [r2]
        pop {r0, r2, r3, pc}
osdNV1: cmp r0, #OSD_VIDEO2
         bne osdNV2
        ldr r2, =0x30682
        ldrh r3, [r2]
        lsl r3, #24
        lsr r3, #24
        lsl r1, #8
        orr r1, r3
        strh r1, [r2]
        pop {r0, r2, r3, pc}         
osdNV2: lsl r0, #1
        ldr r2, =0x30680
        strh r1, [r0, r2]
        pop {r0, r2, r3, pc}
        


@ ------------------------------------------------------------------------------
@ osdSetBrightness r0=(0-255)
@
.globl osdSetBrightness
.thumb_func

osdSetBrightness:
        push {r0, r1, r2, lr}
        ldr r1, =0x30830
        ldrh r2, [r1]
        lsr r2, #8
        lsl r2, #8
        orr r2, r0
        strh r2, [r1]
        pop {r0, r1, r2, pc}


@ ------------------------------------------------------------------------------
@ osdSetContrast r0=(0-255)
@
.globl osdSetContrast
.thumb_func

osdSetContrast:
        push {r0, r1, r2, lr}
        ldr r1, =0x30830
        ldrh r2, [r1]
        lsl r2, #24
        lsr r2, #24
        lsl r0, #8
        orr r2, r0
        strh r2, [r1]
        pop {r0, r1, r2, pc}

@ ------------------------------------------------------------------------------
@ UNCLEANED...
@ ------------------------------------------------------------------------------        


@ osdInit
@
.globl osdInit
.thumb_func
osdInit:
        push {r0, r1, r2, lr}

        mov r2, #0
        ldr r0, =0x30800
        ldr r1, =0x4071
        strh r1, [r0]
        ldr r1, =0x0f40
        strh r1, [r0, #0x2]
        ldr r1, =0x8100
        strh r1, [r0, #0x4]
        ldr r1, =0x4c0f
        strh r1, [r0, #0x6]
        ldr r1, =0xaaaa
        strh r1, [r0, #0x8]
        strh r2, [r0, #0xa]
        strh r2, [r0, #0xc]
        strh r2, [r0, #0xe]
        strh r2, [r0, #0x10]
        strh r2, [r0, #0x12]
        strh r2, [r0, #0x14]
        strh r2, [r0, #0x16]
        strh r2, [r0, #0x18]
        ldr r1, =0x02e0
        strh r1, [r0, #0x1a]
        ldr r1, =0x0085             @ X offset for display
        strh r1, [r0, #0x1c]
        ldr r1, =0x0280             @ WIDTH in pixels (Number processed)
        strh r1, [r0, #0x1e]
        ldr r1, =0x0103
        strh r1, [r0, #0x20]
        ldr r1, =0x0013             @ Y offset for display
        strh r1, [r0, #0x22]
        ldr r1, =0x00f0             @ HEIGHT in pixels (number processed)
        strh r1, [r0, #0x24]
        strh r2, [r0, #0x26]
        strh r2, [r0, #0x28]
        strh r2, [r0, #0x2a]
        ldr r1, =0x8001
        strh r1, [r0, #0x2c]
        ldr r1, =0x0002
        strh r1, [r0, #0x2e]
        ldr r1, =0xff00             @ Brightness adder (lo)
        strh r1, [r0, #0x30]
        strh r2, [r0, #0x32]
        strh r2, [r0, #0x34]
        strh r2, [r0, #0x36]
        ldr r1, =0x0022
        strh r1, [r0, #0x38]
        
        ldr r0, =02600200
        ldr r1, =0x4
        strh r1, [r0]

        ldr r0, =0x30804
        ldr r1, =0x8100
        strh r1, [r0]

        ldr r0, =02600200
        ldr r1, =0x4
        strh r1, [r0]
        pop {r0, r1, r2, pc}

        .ltorg
        .arm
        
.endif
