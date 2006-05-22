        .align 4

.equ MAX_CACHED,    8

.equ REG_STAT,    0x02
.equ REGF_STAT_MAXSPRITE,  0x20
.equ REGF_STAT_STRIKE,     0x40
.equ REGF_CTRL0_OBJADDR,   0x08
.equ REG_CTRL0,            0x00
.equ REGF_CTRL0_OBJ16,     0x20
.equ REG_CTRL1,            0x01

.equ REGF_CTRL1_OBJON,     0x10
.equ REGF_CTRL1_OBJNOCLIP, 0x04

.equ OAMF_SPRITE_0, 0x10
.equ OAMF_VFLIP,    0x80
.equ OAMF_HFLIP,    0x40
.equ OAMF_BEHIND,   0x20

        .globl ppu_cacheOAMa
        .globl ppu_renderOAMLinea

        /*IMPORT                ppuScanline
        IMPORT          ppuRegs
        IMPORT          oamMemory
        IMPORT          oamCache
        IMPORT          oamNumCached
        IMPORT          ppu_sprite0
        IMPORT          ppu_StrikeCycle
        IMPORT          ppu_Latch_FDFE
        IMPORT          ppuTileXOfs*/

.section ".core"

@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ppu_cacheOAMa 
@------------- 
@   input : r0 = PPUPageIndex pointer table
@            r1 = max cached sprite (8 on real nes)
@   output : none
@   + Fill in oamCache with sprite 2 draw on next line
@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@ C structure used :
@
@typedef struct
@{
@   uint8    y;          +0
@   uint8    tile;        +1
@   uint8    attrib;     +2
@   uint8    x;          +3
@}  obj_t;
@oamMemory

@typedef struct
@{
@   uint8    attrib;  +0  // 3 bits from hardware, plus sprite 0 flag    
@   uint8    x;       +1
@   uint8    pat0;    +2
@   uint8    pat1;    +3
@} oamtemp_t;

@ REGISTERS Usage
@ r4  = ppuPageIndex
@ r5  = oamCache
@ r6  = scanline
@ r7  = count
@ r8  = offset
@ r9  = data_ptr
@ r10 = vram_addr
@ r11 = oamNumCached
@ r12 = vram_base


ppu_cacheOAMa:
        stmfd     r13!,{r4-r11}
        
        mov       r4,r0         
        mov       r3,r1
        
        ldr       r6, = ppuScanline
        ldr       r6,[r6]
        ldr       r0, = ppuRegs
        
@clear sprite max flag  
        ldrb      r1,[r0,#REG_STAT]
        bic       r1,r1,#REGF_STAT_MAXSPRITE
        strb      r1,[r0,#REG_STAT]
@       
@oamNumCached = 0
        mov       r11,#0        
@
@compute vram_base      
        ldrb      r1, [r0,#REG_CTRL0]
        tst       r1,#REGF_CTRL0_OBJADDR
        movne     r12, #0x1000
        moveq     r12, #0
@counter and pointers
        ldr       r0, = oamMemory                               
        ldr       r5, = oamCache
        mov       r7,#64   @sprite count max
@check sprite height (8 or 16)  
        tst       r1,#REGF_CTRL0_OBJ16
        bne       ppu_cacheOAM16
@       
@sprite is 8x8
ppu_cacheOAM8:

    @check y    
        ldrb      r8, [r0,#0]
        sub       r8,r6,r8
        cmp       r8,#7
        bhi       nextspr8
        
    @store x
        ldrb      r1,[r0,#3]
        strb      r1,[r5,#1]
        
   @store attrib with sprite0 flag
   @and check if vertical mirror
        ldrb      r1,[r0,#2]
        
        tst       r1,#OAMF_VFLIP    @inverse vertically ?
        eorne     r8,r8,#7    @if yes, inverse vert. offset
        
        tst       r7,#64      @first time ?
        orrne     r1,r1,#OAMF_SPRITE_0   @sprite0 flag                
        biceq     r1,r1,#OAMF_SPRITE_0
        strb      r1,[r5,#0]           
   
        
   @compute vram_addr = vram_base + tile*16
        ldrb      r1,[r0,#1]                                                 
        add           r10,r12,r1,lsl #4 
        
                ldr       r2, = ppu_Latch_FDFE
        ldr               r2,[r2]
                orrs      r2,r2,r2
        beq           spr8_noFDFE
    @latch_fdfe func activated
        and           r1,r10,#0xFC0
        cmp           r1,#0xFC0     
        bne               spr8_noFDFE
    @have to test for FD  or FE
        
        and               r1,r10,#0xFF0
        cmp           r1,#0xFD0
        cmpne     r1,#0xFE0
        bne           spr8_noFDFE
    @have to call latch_fdfe func
        stmfd     r13!,{r0-r12,lr}
        mov               r0,r10
        ldr               lr, = spr8_FDFE_ret
        ldr       r2, = ppu_Latch_FDFE
        ldr               pc,[r2]   
spr8_FDFE_ret:
        ldmfd     r13!,{r0-r12,lr}
spr8_noFDFE:

        
        mov       r2,r10,lsr #10
        bic       r9,r10,#0x1C00
        ldr       r1,[r4,r2,lsl #2]   @ PPUPageIndex[vram_addr>>10]     
        add       r9,r9,r8
        add       r9,r9,r1
        
        ldrb      r1,[r9,#0]
        ldrb      r2,[r9,#8]
        
        strb      r1,[r5,#2]
        strb      r2,[r5,#3]
        
        add       r5,r5,#4
        add       r11,r11,#1
        cmp       r11,#MAX_CACHED   @ max_cached
        ldreq     r2, = ppuRegs
        ldreqb    r1,[r2,#REG_STAT]
        orreq     r1,r1,#REGF_STAT_MAXSPRITE
        streqb    r1,[r2,#REG_STAT]     
        cmp       r11, r3
        beq       end_ppu_cacheOAM
nextspr8:
        add       r0,r0,#4
        subs      r7,r7,#1        
        bne       ppu_cacheOAM8
        b         end_ppu_cacheOAM
        
@sprite is 8x16 
ppu_cacheOAM16:
    @check y    
        ldrb      r8, [r0,#0]
        sub       r8,r6,r8
        cmp       r8,#15
        bhi       nextspr16
        
        tst       r8,#8
        addne     r8,r8,#8
        
    @store x
        ldrb      r1,[r0,#3]
        strb      r1,[r5,#1]
        
   @store attrib with sprite0 flag
   @and check if vertical mirror
        ldrb      r1,[r0,#2]
        
        tst       r1,#OAMF_VFLIP    @inverse vertically ?
        eorne     r8,r8,#23    @if yes, inverse vert. offset
        
        tst       r7,#64      @first time ?
        orrne     r1,r1,#OAMF_SPRITE_0   @sprite0 flag                
        biceq     r1,r1,#OAMF_SPRITE_0
        strb      r1,[r5,#0]              
        
   @compute vram_addr = (tile&1)<<12 | (tile & 0xfe)<<4
   @                     -vram_base-   
        ldrb      r1,[r0,#1]    
        tst       r1,#1
        movne     r12,#0x1000
        moveq     r12,#0
        bic       r1,r1,#1                                      
        
        orr       r10,r12,r1,lsl #4
        
        ldr       r2, = ppu_Latch_FDFE
        ldr               r2,[r2]
                orrs      r2,r2,r2
        beq           spr16_noFDFE      
    @latch_fdfe func activated
        and           r1,r10,#0xFC0
        cmp           r1,#0xFC0     
        bne               spr16_noFDFE
    @have to test for FD or FE          
        and               r1,r10,#0xFF0
        cmp           r1,#0xFD0
        cmpne     r1,#0xFE0
        bne           spr16_noFDFE
    @have to call latch_fdfe func
        stmfd     r13!,{r0-r12,lr}
        mov               r0,r10
        ldr               lr, = spr16_FDFE_ret
        ldr       r2, = ppu_Latch_FDFE
        ldr               pc,[r2]
spr16_FDFE_ret:
        ldmfd     r13!,{r0-r12,lr}
spr16_noFDFE:
                
        mov       r2,r10,lsr #10
        bic       r9,r10,#0x1C00
        ldr       r1,[r4,r2,lsl #2]   @ PPUPageIndex[vram_addr>>10]                             
        add       r9,r9,r8
        add       r9,r9,r1
        
        ldrb      r1,[r9,#0]
        ldrb      r2,[r9,#8]
        
        strb      r1,[r5,#2]
        strb      r2,[r5,#3]
        
        add       r5,r5,#4
        add       r11,r11,#1
        cmp       r11,#MAX_CACHED   @ max_cached
        ldreq     r2, = ppuRegs
        ldreqb    r1,[r2,#REG_STAT]
        orreq     r1,r1,#REGF_STAT_MAXSPRITE
        streqb    r1,[r2,#REG_STAT]     
        cmp       r11,r3
        beq       end_ppu_cacheOAM
nextspr16:
        add       r0,r0,#4
        subs      r7,r7,#1
        bne       ppu_cacheOAM16
                
end_ppu_cacheOAM:

        ldr       r0, = oamNumCached
        str       r11,[r0]

        ldmfd     r13!,{r4-r11}
        mov           pc,lr
@@@@@@@@@@@@@@@@@@


@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ppu_renderOAMLinea
@------------------
@   input : r0 = drawing buffer  (8+256+8 bytes long)
@   output : none
@   + Draw the sprites cached on previous line
@@@@@@@@@@@@@@@@@@@@@@@@@@@@    

@ REGISTERS Usage

@ r6 =  colRow
@ r7  = priority_buf + x + 8
@ r8  = buffer + x + 8
@ r9  = pattern
@ r10 = oamCache
@ r11 = counter
@ r12 = buffer

ppu_renderOAMLinea:
        stmfd     r13!,{r4-r11}
                        
        mov       r12,r0                
        
        ldr       r0, = ppuRegs
        ldrb      r1,[r0,#REG_CTRL1]
 @test if sprite drawing is on
        tst       r1,#REGF_CTRL1_OBJON
        beq       end_ppu_renderOAMLinea 
 @test if sprite left clipping is on    
        tst       r1,#REGF_CTRL1_OBJNOCLIP
        bne       DontClip1
@       ldreq     r1,[r12,#8]
@       ldreq     r2,[r12,#12]
@       stmeqfd   r13!,{r1-r2}
        ldr        r6, = ppuTileXOfs
        ldr        r6,[r6]
        add        r6,r6,r12
        ldrb   r3,[r6,#8]
        mov        r1,r3
        ldrb   r3,[r6,#9]
        orr        r1,r3,r1
        ldrb   r3,[r6,#10]
        orr        r1,r3,r1
        ldrb   r3,[r6,#11]
        orr        r1,r3,r1
        ldrb   r3,[r6,#12]
        mov        r2,r3
        ldrb   r3,[r6,#13]
        orr        r2,r3,r1
        ldrb   r3,[r6,#14]
        orr        r2,r3,r1
        ldrb   r3,[r6,#15]
        orr        r2,r3,r1

        stmfd  r13!,{r1-r2}
DontClip1:
        
  @reset sprite0hit
        ldr       r2, = sprite0hit
        mov       r3,#0
        strb      r3,[r2]
        
 @zerod priority buffer
        ldr       r1, = priority_buf
        mov       r2,#0
        .REPT	  68
        	str	  r2,[r1],#4
	      .ENDR
 @init sprite cache pointer
        ldr       r10, = oamCache
 @init sprite counter   
        ldr       r11, = oamNumCached
        ldr       r11,[r11]
        
 @check if we have to check sprite0 collision
        ldrb      r1,[r0,#REG_STAT]
        tst       r1,#REGF_STAT_STRIKE
        beq       ppu_renderOAMLinea_check0 
 @dont check sprite0    
ppu_renderOAMLinea_Nocheck0:

        ldrb      r0,[r10,#2]   @r0 = pat0
        ldrb      r1,[r10,#3]   @r1 = pat1

@pattern = ((pat1 & 0xaa) << 8) | ((pat1 & 0x55) << 1) | ((pat0 & 0xaa) << 7) | (pat0 & 0x55);
@                r2                    r3                       r4                 r5

        and       r2,r1,#0xaa
        and       r3,r1,#0x55
        and       r4,r0,#0xaa
        and       r5,r0,#0x55
        
        orr       r0,r3,r2,lsl #7
        orr       r1,r5,r4,lsl #7                       
        orrs      r9,r1,r0,lsl #1
  @ is 100% transparent ?               
  @ if yes, skip and next sprite
        beq       ppu_renderOAMLinea_Nocheck0NextSprite
        
  @ init r7 = priority_buf + x + 8
  @  and r8 = buffer + x + 8
        ldrb      r8,[r10,#1]
        ldr       r7, = priority_buf
        
        ldr       r6, = ppuTileXOfs
        ldr       r6,[r6]
        orr       r6,r6,#8
        
        add       r7,r7,r8 @#8
        add       r7,r7,r6
        add       r8,r12,r8
        add       r8,r8,r6  @#8
        
                                
  @ load attrib
        ldrb      r6,[r10,#0]
        tst       r6,#OAMF_HFLIP
   @ colRow = (attrib & 3) * 4 + 16
        bic       r6,r6,#0xFC
        mov       r6,r6,lsl #2
        orr       r6,r6,#0x10
        
   @init pixels | colrow
   @  r5 -> 3 2 1 0
   @  r4 -> 7 6 5 4
        bne       ppu_renderOAMLinea_Nocheck0Hflip
     @pixels[0] = (pattern >> 14) & 3;
        mov       r0,r9,lsr #14
        and       r0,r0,#0x03
        orr       r5,r0,r6
     @pixels[1] = (pattern >> 6) & 3;
        mov       r0,r9,lsl #2
        and       r0,r0,#0x0300
        orr       r0,r0,r6,lsl #8
        orr       r5,r5,r0
     @pixels[2] = (pattern >> 12) & 3;
        mov       r0,r9,lsl #4
        and       r0,r0,#0x030000
        orr       r0,r0,r6,lsl #16
        orr       r5,r5,r0
     @pixels[3] = (pattern >> 4) & 3;
        mov       r0,r9,lsl #20
        and       r0,r0,#0x03000000
        orr       r0,r0,r6,lsl #24
        orr       r5,r5,r0
     @pixels[4] = (pattern >> 10) & 3;
        mov       r0,r9,lsr #10
        and       r0,r0,#0x03
        orr       r4,r0,r6
     @pixels[5] = (pattern >> 2) & 3;
        mov       r0,r9,lsl #6
        and       r0,r0,#0x0300
        orr       r0,r0,r6,lsl #8
        orr       r4,r4,r0
     @pixels[6] = (pattern >> 8) & 3;
        mov       r0,r9,lsl #8
        and       r0,r0,#0x030000
        orr       r0,r0,r6,lsl #16
        orr       r4,r4,r0
     @pixels[7] = pattern & 3
        mov       r0,r9,lsl #24
        and       r0,r0,#0x03000000
        orr       r0,r0,r6,lsl #24
        orr       r4,r4,r0              
        b         ppu_renderOAMLinea_Nocheck0NoHflip
ppu_renderOAMLinea_Nocheck0Hflip:
     @pixels[0] = pattern & 3
        mov       r0,r9
        and       r0,r0,#0x03
        orr       r5,r0,r6
     @pixels[1] = (pattern >> 8) & 3;
        mov       r0,r9
        and       r0,r0,#0x0300
        orr       r0,r0,r6,lsl #8
        orr       r5,r5,r0
     @pixels[2] = (pattern >> 2) & 3;
        mov       r0,r9,lsl #14
        and       r0,r0,#0x030000
        orr       r0,r0,r6,lsl #16
        orr       r5,r5,r0
     @pixels[3] = (pattern >> 10) & 3;
        mov       r0,r9,lsl #14
        and       r0,r0,#0x03000000
        orr       r0,r0,r6,lsl #24
        orr       r5,r5,r0
     @pixels[4] = (pattern >> 4) & 3;
        mov       r0,r9,lsr #4
        and       r0,r0,#0x03
        orr       r4,r0,r6
     @pixels[5] = (pattern >> 12) & 3;
        mov       r0,r9,lsr #4
        and       r0,r0,#0x0300
        orr       r0,r0,r6,lsl #8
        orr       r4,r4,r0
     @pixels[6] = (pattern >> 6) & 3;           
        mov       r0,r9,lsl #10
        and       r0,r0,#0x030000
        orr       r0,r0,r6,lsl #16
        orr       r4,r4,r0
     @pixels[7] = (pattern >> 14) & 3;
        mov       r0,r9,lsl #10
        and       r0,r0,#0x03000000
        orr       r0,r0,r6,lsl #24
        orr       r4,r4,r0              
        b         ppu_renderOAMLinea_Nocheck0NoHflip
ppu_renderOAMLinea_Nocheck0NoHflip:
                  
        ldrb      r0,[r10,#0]
  @little hack to stop testing sprite0 collision with bg when done
Sprite0Hit:
        tst       r0,#OAMF_BEHIND
        bne       ppu_renderOAMLinea_Nocheck0DrawBehind
        
  @init priority_buf registers
  @ r3 -> pixels  3 2 1 0
  @ r2 -> pixels  7 5 4 3
        ldrb      r0,[r7,#0]
        mov       r3,r0
        ldrb      r0,[r7,#1]
        orr       r3,r3,r0,lsl #8
        ldrb      r0,[r7,#2]
        orr       r3,r3,r0,lsl #16
        ldrb      r0,[r7,#3]
        orr       r3,r3,r0,lsl #24      
        
        ldrb      r0,[r7,#4]
        mov       r2,r0
        ldrb      r0,[r7,#5]
        orr       r2,r2,r0,lsl #8
        ldrb      r0,[r7,#6]
        orr       r2,r2,r0,lsl #16
        ldrb      r0,[r7,#7]
        orr       r2,r2,r0,lsl #24      
  
    @pixels[0]
    @ is pixel transparent ?
        tst       r5,#0x03
        beq       ppu_renderOAMLinea_Nocheck0DrawFrontPix1
    @ is priority_buf == 0 ?
        tst       r3,#0xFF
        bne       ppu_renderOAMLinea_Nocheck0DrawFrontPix1
    @ draw pixel & update priority_buf                  
        strb      r5,[r8,#0]  @ pBuf[pix] = (colRow | pixels[pix])
        strb      r5,[r7,#0]  @ pPriority[pix] = 1 (in fact non 0)
ppu_renderOAMLinea_Nocheck0DrawFrontPix1:
    @pixels[1]
    @ is pixel transparent ?
        tst       r5,#0x0300
        beq       ppu_renderOAMLinea_Nocheck0DrawFrontPix2
    @ is priority_buf == 0 ?
        tst       r3,#0xFF00
        bne       ppu_renderOAMLinea_Nocheck0DrawFrontPix2
    @ draw pixel & update priority_buf          
        mov       r0,r5,lsr #8    
        strb      r0,[r8,#1]  @ pBuf[pix] = (colRow | pixels[pix])
        strb      r0,[r7,#1]  @ pPriority[pix] = 1 (in fact non 0)
ppu_renderOAMLinea_Nocheck0DrawFrontPix2:
    @pixels[2]
    @ is pixel transparent ?
        tst       r5,#0x030000
        beq       ppu_renderOAMLinea_Nocheck0DrawFrontPix3
    @ is priority_buf == 0 ?
        tst       r3,#0xFF0000
        bne       ppu_renderOAMLinea_Nocheck0DrawFrontPix3
    @ draw pixel & update priority_buf          
        mov       r0,r5,lsr #16
        strb      r0,[r8,#2]  @ pBuf[pix] = (colRow | pixels[pix])
        strb      r0,[r7,#2]  @ pPriority[pix] = 1 (in fact non 0)
ppu_renderOAMLinea_Nocheck0DrawFrontPix3:
    @pixels[3]
    @ is pixel transparent ?
        tst       r5,#0x03000000
        beq       ppu_renderOAMLinea_Nocheck0DrawFrontPix4
    @ is priority_buf == 0 ?
        tst       r3,#0xFF000000
        bne       ppu_renderOAMLinea_Nocheck0DrawFrontPix4
    @ draw pixel & update priority_buf          
        mov       r0,r5,lsr #24
        strb      r0,[r8,#3]  @ pBuf[pix] = (colRow | pixels[pix])
        strb      r0,[r7,#3]  @ pPriority[pix] = 1 (in fact non 0)
ppu_renderOAMLinea_Nocheck0DrawFrontPix4:
    @pixels[4]
    @ is pixel transparent ?
        tst       r4,#0x03
        beq       ppu_renderOAMLinea_Nocheck0DrawFrontPix5
    @ is priority_buf == 0 ?
        tst       r2,#0xFF
        bne       ppu_renderOAMLinea_Nocheck0DrawFrontPix5
    @ draw pixel & update priority_buf          
        strb      r4,[r8,#4]  @ pBuf[pix] = (colRow | pixels[pix])
        strb      r4,[r7,#4]  @ pPriority[pix] = 1 (in fact non 0)
ppu_renderOAMLinea_Nocheck0DrawFrontPix5:
    @pixels[5]
    @ is pixel transparent ?
        tst       r4,#0x0300
        beq       ppu_renderOAMLinea_Nocheck0DrawFrontPix6
    @ is priority_buf == 0 ?
        tst       r2,#0xFF00
        bne       ppu_renderOAMLinea_Nocheck0DrawFrontPix6
    @ draw pixel & update priority_buf          
        mov       r0,r4,lsr #8    
        strb      r0,[r8,#5]  @ pBuf[pix] = (colRow | pixels[pix])
        strb      r0,[r7,#5]  @ pPriority[pix] = 1 (in fact non 0)
ppu_renderOAMLinea_Nocheck0DrawFrontPix6:
    @pixels[6]
    @ is pixel transparent ?
        tst       r4,#0x030000
        beq       ppu_renderOAMLinea_Nocheck0DrawFrontPix7
    @ is priority_buf == 0 ?
        tst       r2,#0xFF0000
        bne       ppu_renderOAMLinea_Nocheck0DrawFrontPix7
    @ draw pixel & update priority_buf          
        mov       r0,r4,lsr #16
        strb      r0,[r8,#6]  @ pBuf[pix] = (colRow | pixels[pix])
        strb      r0,[r7,#6]  @ pPriority[pix] = 1 (in fact non 0)
ppu_renderOAMLinea_Nocheck0DrawFrontPix7:
    @pixels[7]
    @ is pixel transparent ?
        tst       r4,#0x03000000
        beq       ppu_renderOAMLinea_Nocheck0DrawFrontPixEnd
    @ is priority_buf == 0 ?
        tst       r2,#0xFF000000
        bne       ppu_renderOAMLinea_Nocheck0DrawFrontPixEnd
    @ draw pixel & update priority_buf          
        mov       r0,r4,lsr #24
        strb      r0,[r8,#7]  @ pBuf[pix] = (colRow | pixels[pix])
        strb      r0,[r7,#7]  @ pPriority[pix] = 1 (in fact non 0)
ppu_renderOAMLinea_Nocheck0DrawFrontPixEnd:
        b         ppu_renderOAMLinea_Nocheck0NextSprite
        
ppu_renderOAMLinea_Nocheck0DrawBehind:

  @init pBuf registers
  @ r3 -> pixels  3 2 1 0
  @ r2 -> pixels  7 5 4 3
        ldrb      r0,[r8,#0]
        mov       r3,r0
        ldrb      r0,[r8,#1]
        orr       r3,r3,r0,lsl #8
        ldrb      r0,[r8,#2]
        orr       r3,r3,r0,lsl #16
        ldrb      r0,[r8,#3]
        orr       r3,r3,r0,lsl #24      
        
        ldrb      r0,[r8,#4]
        mov       r2,r0
        ldrb      r0,[r8,#5]
        orr       r2,r2,r0,lsl #8
        ldrb      r0,[r8,#6]
        orr       r2,r2,r0,lsl #16
        ldrb      r0,[r8,#7]
        orr       r2,r2,r0,lsl #24      
  
    @pixels[0]
    @ is pixel transparent ?
        tst       r5,#0x03
        beq       ppu_renderOAMLinea_Nocheck0DrawBehindPix1
    @ is priority_buf == 0 ?
        tst       r3,#0x03      
    @ draw pixel & update priority_buf                  
        streqb    r5,[r8,#0]  @ pBuf[pix] = (colRow | pixels[pix])
        strb      r5,[r7,#0]  @ pPriority[pix] = 1 (in fact non 0)
ppu_renderOAMLinea_Nocheck0DrawBehindPix1:
    @pixels[1]
    @ is pixel transparent ?
        tst       r5,#0x0300
        beq       ppu_renderOAMLinea_Nocheck0DrawBehindPix2
    @ is priority_buf == 0 ?        
        tst       r3,#0x0300    
    @ draw pixel & update priority_buf                  
        mov       r0,r5,lsr #8    
        streqb    r0,[r8,#1]  @ pBuf[pix] = (colRow | pixels[pix])
        strb      r0,[r7,#1]  @ pPriority[pix] = 1 (in fact non 0)
ppu_renderOAMLinea_Nocheck0DrawBehindPix2:
    @pixels[2]
    @ is pixel transparent ?
        tst       r5,#0x030000
        beq       ppu_renderOAMLinea_Nocheck0DrawBehindPix3
    @ is priority_buf == 0 ?
        tst       r3,#0x030000          
    @ draw pixel & update priority_buf          
        mov       r0,r5,lsr #16
        streqb    r0,[r8,#2]  @ pBuf[pix] = (colRow | pixels[pix])
        strb      r0,[r7,#2]  @ pPriority[pix] = 1 (in fact non 0)
ppu_renderOAMLinea_Nocheck0DrawBehindPix3:
    @pixels[3]
    @ is pixel transparent ?
        tst       r5,#0x03000000
        beq       ppu_renderOAMLinea_Nocheck0DrawBehindPix4
    @ is priority_buf == 0 ?
        tst       r3,#0x03000000        
    @ draw pixel & update priority_buf          
        mov       r0,r5,lsr #24
        streqb    r0,[r8,#3]  @ pBuf[pix] = (colRow | pixels[pix])
        strb      r0,[r7,#3]  @ pPriority[pix] = 1 (in fact non 0)
ppu_renderOAMLinea_Nocheck0DrawBehindPix4:
    @pixels[4]
    @ is pixel transparent ?
        tst       r4,#0x03
        beq       ppu_renderOAMLinea_Nocheck0DrawBehindPix5
    @ is priority_buf == 0 ?
        tst       r2,#0x03      
    @ draw pixel & update priority_buf                  
        streqb    r4,[r8,#4]  @ pBuf[pix] = (colRow | pixels[pix])
        strb      r4,[r7,#4]  @ pPriority[pix] = 1 (in fact non 0)
ppu_renderOAMLinea_Nocheck0DrawBehindPix5:
    @pixels[5]
    @ is pixel transparent ?
        tst       r4,#0x0300
        beq       ppu_renderOAMLinea_Nocheck0DrawBehindPix6
    @ is priority_buf == 0 ?
        tst       r2,#0x0300    
    @ draw pixel & update priority_buf          
        mov       r0,r4,lsr #8    
        streqb    r0,[r8,#5]  @ pBuf[pix] = (colRow | pixels[pix])
        strb      r0,[r7,#5]  @ pPriority[pix] = 1 (in fact non 0)
ppu_renderOAMLinea_Nocheck0DrawBehindPix6:
    @pixels[6]
    @ is pixel transparent ?
        tst       r4,#0x030000
        beq       ppu_renderOAMLinea_Nocheck0DrawBehindPix7
    @ is priority_buf == 0 ?
        tst       r2,#0x030000          
    @ draw pixel & update priority_buf          
        mov       r0,r4,lsr #16
        streqb    r0,[r8,#6]  @ pBuf[pix] = (colRow | pixels[pix])
        strb      r0,[r7,#6]  @ pPriority[pix] = 1 (in fact non 0)
ppu_renderOAMLinea_Nocheck0DrawBehindPix7:
    @pixels[7]
    @ is pixel transparent ?
        tst       r4,#0x03000000
        beq       ppu_renderOAMLinea_Nocheck0DrawBehindPixEnd
    @ is priority_buf == 0 ?
        tst       r2,#0x03000000        
    @ draw pixel & update priority_buf          
        mov       r0,r4,lsr #24
        streqb    r0,[r8,#7]  @ pBuf[pix] = (colRow | pixels[pix])
        strb      r0,[r7,#7]  @ pPriority[pix] = 1 (in fact non 0)
ppu_renderOAMLinea_Nocheck0DrawBehindPixEnd:
          
        b         ppu_renderOAMLinea_Nocheck0NextSprite
  
  @next sprite  
ppu_renderOAMLinea_Nocheck0NextSprite:
        add       r10,r10,#4
  @is it the last ?
        subs      r11,r11,#1
        bne       ppu_renderOAMLinea_Nocheck0   
        b         ppu_renderOAMLineaCheckClip
 @check sprite0
ppu_renderOAMLinea_check0:
        ldrb      r0,[r10,#2]   @r0 = pat0
        ldrb      r1,[r10,#3]   @r1 = pat1

@pattern = ((pat1 & 0xaa) << 8) | ((pat1 & 0x55) << 1) | ((pat0 & 0xaa) << 7) | (pat0 & 0x55);
@                r2                    r3                       r4                 r5

        and       r2,r1,#0xaa
        and       r3,r1,#0x55
        and       r4,r0,#0xaa
        and       r5,r0,#0x55
        
        orr       r0,r3,r2,lsl #7
        orr       r1,r5,r4,lsl #7                       
        orrs      r9,r1,r0,lsl #1
  @ is 100% transparent ?               
  @ if yes, skip and next sprite
        beq       ppu_renderOAMLinea_check0NextSprite
        
  @ init r7 = priority_buf + x + 8
  @  and r8 = buffer + x + 8
        ldrb      r8,[r10,#1]
        ldr       r7, = priority_buf
        
        ldr       r6, = ppuTileXOfs
        ldr       r6,[r6]
        orr       r6,r6,#8
        
        add       r7,r7,r8
        add       r7,r7,r6 @#8
        add       r8,r12,r8
        add       r8,r8,r6  @#8
        
                                
  @ load attrib
        ldrb      r6,[r10,#0]
        tst       r6,#OAMF_HFLIP
   @ colRow = (attrib & 3) * 4 + 16
        bic       r6,r6,#0xFC
        mov       r6,r6,lsl #2
        orr       r6,r6,#0x10
        
   @init pixels | colrow
   @  r5 -> 3 2 1 0
   @  r4 -> 7 6 5 4
        bne       ppu_renderOAMLinea_check0Hflip        
     @pixels[0] = (pattern >> 14) & 3;
        mov       r0,r9,lsr #14
        and       r0,r0,#0x03
        orr       r5,r0,r6
     @pixels[1] = (pattern >> 6) & 3;           
        mov       r0,r9,lsl #2
        and       r0,r0,#0x0300
        orr       r0,r0,r6,lsl #8
        orr       r5,r5,r0
     @pixels[2] = (pattern >> 12) & 3;
        mov       r0,r9,lsl #4
        and       r0,r0,#0x030000
        orr       r0,r0,r6,lsl #16
        orr       r5,r5,r0
     @pixels[3] = (pattern >> 4) & 3;
        mov       r0,r9,lsl #20
        and       r0,r0,#0x03000000
        orr       r0,r0,r6,lsl #24
        orr       r5,r5,r0
     @pixels[4] = (pattern >> 10) & 3;
        mov       r0,r9,lsr #10
        and       r0,r0,#0x03
        orr       r4,r0,r6
     @pixels[5] = (pattern >> 2) & 3;
        mov       r0,r9,lsl #6
        and       r0,r0,#0x0300
        orr       r0,r0,r6,lsl #8
        orr       r4,r4,r0
     @pixels[6] = (pattern >> 8) & 3;
        mov       r0,r9,lsl #8
        and       r0,r0,#0x030000
        orr       r0,r0,r6,lsl #16
        orr       r4,r4,r0
     @pixels[7] = pattern & 3
        mov       r0,r9,lsl #24
        and       r0,r0,#0x03000000
        orr       r0,r0,r6,lsl #24
        orr       r4,r4,r0              
        b         ppu_renderOAMLinea_check0NoHflip
ppu_renderOAMLinea_check0Hflip:
     @pixels[0] = pattern & 3
        mov       r0,r9
        and       r0,r0,#0x03
        orr       r5,r0,r6
     @pixels[1] = (pattern >> 8) & 3;
        mov       r0,r9
        and       r0,r0,#0x0300
        orr       r0,r0,r6,lsl #8
        orr       r5,r5,r0
     @pixels[2] = (pattern >> 2) & 3;
        mov       r0,r9,lsl #14
        and       r0,r0,#0x030000
        orr       r0,r0,r6,lsl #16
        orr       r5,r5,r0
     @pixels[3] = (pattern >> 10) & 3;
        mov       r0,r9,lsl #14
        and       r0,r0,#0x03000000
        orr       r0,r0,r6,lsl #24
        orr       r5,r5,r0
     @pixels[4] = (pattern >> 4) & 3;
        mov       r0,r9,lsr #4
        and       r0,r0,#0x03
        orr       r4,r0,r6
     @pixels[5] = (pattern >> 12) & 3;
        mov       r0,r9,lsr #4
        and       r0,r0,#0x0300
        orr       r0,r0,r6,lsl #8
        orr       r4,r4,r0
     @pixels[6] = (pattern >> 6) & 3;           
        mov       r0,r9,lsl #10
        and       r0,r0,#0x030000
        orr       r0,r0,r6,lsl #16
        orr       r4,r4,r0
     @pixels[7] = (pattern >> 14) & 3;
        mov       r0,r9,lsl #10
        and       r0,r0,#0x03000000
        orr       r0,r0,r6,lsl #24
        orr       r4,r4,r0              
        b         ppu_renderOAMLinea_check0NoHflip
ppu_renderOAMLinea_check0NoHflip:


                          
        ldrb      r0,[r10,#0]
        tst       r0,#OAMF_SPRITE_0
        beq       NoCheck0

        ldr       r2, = sprite0hit
        mov       r3,#1
        
        tst       r5,#0x03
        ldrneb    r1,[r8,#0]
        tstne     r1,#3
        strneb    r3,[r2]
        bne       Sprite0Hit
        tst       r5,#0x0300
        ldrneb    r1,[r8,#1]
        tstne     r1,#3
        strneb    r3,[r2]
        bne       Sprite0Hit
        tst       r5,#0x030000
        ldrneb    r1,[r8,#2]
        tstne     r1,#3
        strneb    r3,[r2]
        bne       Sprite0Hit
        tst       r5,#0x03000000
        ldrneb    r1,[r8,#3]
        tstne     r1,#3
        strneb    r3,[r2]
        bne       Sprite0Hit
        tst       r4,#0x03
        ldrneb    r1,[r8,#4]
        tstne     r1,#3
        strneb    r3,[r2]
        bne       Sprite0Hit
        tst       r4,#0x0300
        ldrneb    r1,[r8,#5]
        tstne     r1,#3
        strneb    r3,[r2]
        bne       Sprite0Hit
        tst       r4,#0x030000
        ldrneb    r1,[r8,#6]
        tstne     r1,#3
        strneb    r3,[r2]
        bne       Sprite0Hit
        tst       r4,#0x03000000
        ldrneb    r1,[r8,#7]
        tstne     r1,#3
        strneb    r3,[r2]
        bne       Sprite0Hit
NoCheck0:
        tst       r0,#OAMF_BEHIND
        bne       ppu_renderOAMLinea_check0DrawBehind
        
  @init priority_buf registers
  @ r3 -> pixels  3 2 1 0
  @ r2 -> pixels  7 5 4 3
        ldrb      r0,[r7,#0]
        mov       r3,r0
        ldrb      r0,[r7,#1]
        orr       r3,r3,r0,lsl #8
        ldrb      r0,[r7,#2]
        orr       r3,r3,r0,lsl #16
        ldrb      r0,[r7,#3]
        orr       r3,r3,r0,lsl #24      
        
        ldrb      r0,[r7,#4]
        mov       r2,r0
        ldrb      r0,[r7,#5]
        orr       r2,r2,r0,lsl #8
        ldrb      r0,[r7,#6]
        orr       r2,r2,r0,lsl #16
        ldrb      r0,[r7,#7]
        orr       r2,r2,r0,lsl #24      
  
    @pixels[0]
    @ is pixel transparent ?
        tst       r5,#0x03
        beq       ppu_renderOAMLinea_check0DrawFrontPix1
    @ is priority_buf == 0 ?
        tst       r3,#0xFF
        bne       ppu_renderOAMLinea_check0DrawFrontPix1
    @ draw pixel & update priority_buf                  
        strb      r5,[r8,#0]  @ pBuf[pix] = (colRow | pixels[pix])
        strb      r5,[r7,#0]  @ pPriority[pix] = 1 (in fact non 0)
ppu_renderOAMLinea_check0DrawFrontPix1:
    @pixels[1]
    @ is pixel transparent ?
        tst       r5,#0x0300
        beq       ppu_renderOAMLinea_check0DrawFrontPix2
    @ is priority_buf == 0 ?
        tst       r3,#0xFF00
        bne       ppu_renderOAMLinea_check0DrawFrontPix2
    @ draw pixel & update priority_buf          
        mov       r0,r5,lsr #8    
        strb      r0,[r8,#1]  @ pBuf[pix] = (colRow | pixels[pix])
        strb      r0,[r7,#1]  @ pPriority[pix] = 1 (in fact non 0)
ppu_renderOAMLinea_check0DrawFrontPix2:
    @pixels[2]
    @ is pixel transparent ?
        tst       r5,#0x030000
        beq       ppu_renderOAMLinea_check0DrawFrontPix3
    @ is priority_buf == 0 ?
        tst       r3,#0xFF0000
        bne       ppu_renderOAMLinea_check0DrawFrontPix3
    @ draw pixel & update priority_buf          
        mov       r0,r5,lsr #16
        strb      r0,[r8,#2]  @ pBuf[pix] = (colRow | pixels[pix])
        strb      r0,[r7,#2]  @ pPriority[pix] = 1 (in fact non 0)
ppu_renderOAMLinea_check0DrawFrontPix3:
    @pixels[3]
    @ is pixel transparent ?
        tst       r5,#0x03000000
        beq       ppu_renderOAMLinea_check0DrawFrontPix4
    @ is priority_buf == 0 ?
        tst       r3,#0xFF000000
        bne       ppu_renderOAMLinea_check0DrawFrontPix4
    @ draw pixel & update priority_buf          
        mov       r0,r5,lsr #24
        strb      r0,[r8,#3]  @ pBuf[pix] = (colRow | pixels[pix])
        strb      r0,[r7,#3]  @ pPriority[pix] = 1 (in fact non 0)
ppu_renderOAMLinea_check0DrawFrontPix4:
    @pixels[4]
    @ is pixel transparent ?
        tst       r4,#0x03
        beq       ppu_renderOAMLinea_check0DrawFrontPix5
    @ is priority_buf == 0 ?
        tst       r2,#0xFF
        bne       ppu_renderOAMLinea_check0DrawFrontPix5
    @ draw pixel & update priority_buf          
        strb      r4,[r8,#4]  @ pBuf[pix] = (colRow | pixels[pix])
        strb      r4,[r7,#4]  @ pPriority[pix] = 1 (in fact non 0)
ppu_renderOAMLinea_check0DrawFrontPix5:
    @pixels[5]
    @ is pixel transparent ?
        tst       r4,#0x0300
        beq       ppu_renderOAMLinea_check0DrawFrontPix6
    @ is priority_buf == 0 ?
        tst       r2,#0xFF00
        bne       ppu_renderOAMLinea_check0DrawFrontPix6
    @ draw pixel & update priority_buf          
        mov       r0,r4,lsr #8    
        strb      r0,[r8,#5]  @ pBuf[pix] = (colRow | pixels[pix])
        strb      r0,[r7,#5]  @ pPriority[pix] = 1 (in fact non 0)
ppu_renderOAMLinea_check0DrawFrontPix6:
    @pixels[6]
    @ is pixel transparent ?
        tst       r4,#0x030000
        beq       ppu_renderOAMLinea_check0DrawFrontPix7
    @ is priority_buf == 0 ?
        tst       r2,#0xFF0000
        bne       ppu_renderOAMLinea_check0DrawFrontPix7
    @ draw pixel & update priority_buf          
        mov       r0,r4,lsr #16
        strb      r0,[r8,#6]  @ pBuf[pix] = (colRow | pixels[pix])
        strb      r0,[r7,#6]  @ pPriority[pix] = 1 (in fact non 0)
ppu_renderOAMLinea_check0DrawFrontPix7:
    @pixels[7]
    @ is pixel transparent ?
        tst       r4,#0x03000000
        beq       ppu_renderOAMLinea_check0DrawFrontPixEnd
    @ is priority_buf == 0 ?
        tst       r2,#0xFF000000
        bne       ppu_renderOAMLinea_check0DrawFrontPixEnd
    @ draw pixel & update priority_buf          
        mov       r0,r4,lsr #24
        strb      r0,[r8,#7]  @ pBuf[pix] = (colRow | pixels[pix])
        strb      r0,[r7,#7]  @ pPriority[pix] = 1 (in fact non 0)
ppu_renderOAMLinea_check0DrawFrontPixEnd:
        b         ppu_renderOAMLinea_check0NextSprite
        
ppu_renderOAMLinea_check0DrawBehind:

  @init pBuf registers
  @ r3 -> pixels  3 2 1 0
  @ r2 -> pixels  7 5 4 3
        ldrb      r0,[r8,#0]
        mov       r3,r0
        ldrb      r0,[r8,#1]
        orr       r3,r3,r0,lsl #8
        ldrb      r0,[r8,#2]
        orr       r3,r3,r0,lsl #16
        ldrb      r0,[r8,#3]
        orr       r3,r3,r0,lsl #24
                
        ldrb      r0,[r8,#4]
        mov       r2,r0
        ldrb      r0,[r8,#5]
        orr       r2,r2,r0,lsl #8
        ldrb      r0,[r8,#6]
        orr       r2,r2,r0,lsl #16
        ldrb      r0,[r8,#7]
        orr       r2,r2,r0,lsl #24      
  
    @pixels[0]
    @ is pixel transparent ?
        tst       r5,#0x03
        beq       ppu_renderOAMLinea_check0DrawBehindPix1
    @ is priority_buf == 0 ?
        tst       r3,#0x03      
    @ draw pixel & update priority_buf                  
        streqb    r5,[r8,#0]  @ pBuf[pix] = (colRow | pixels[pix])
        strb      r5,[r7,#0]  @ pPriority[pix] = 1 (in fact non 0)
ppu_renderOAMLinea_check0DrawBehindPix1:
    @pixels[1]
    @ is pixel transparent ?
        tst       r5,#0x0300
        beq       ppu_renderOAMLinea_check0DrawBehindPix2
    @ is priority_buf == 0 ?
        tst       r3,#0x0300    
    @ draw pixel & update priority_buf          
        mov       r0,r5,lsr #8    
        streqb    r0,[r8,#1]  @ pBuf[pix] = (colRow | pixels[pix])
        strb      r0,[r7,#1]  @ pPriority[pix] = 1 (in fact non 0)
ppu_renderOAMLinea_check0DrawBehindPix2:
    @pixels[2]
    @ is pixel transparent ?
        tst       r5,#0x030000
        beq       ppu_renderOAMLinea_check0DrawBehindPix3
    @ is priority_buf == 0 ?
        tst       r3,#0x030000  
    @ draw pixel & update priority_buf          
        mov       r0,r5,lsr #16
        streqb    r0,[r8,#2]  @ pBuf[pix] = (colRow | pixels[pix])
        strb      r0,[r7,#2]  @ pPriority[pix] = 1 (in fact non 0)
ppu_renderOAMLinea_check0DrawBehindPix3:
    @pixels[3]
    @ is pixel transparent ?
        tst       r5,#0x03000000
        beq       ppu_renderOAMLinea_check0DrawBehindPix4
    @ is priority_buf == 0 ?
        tst       r3,#0x03000000        
    @ draw pixel & update priority_buf          
        mov       r0,r5,lsr #24
        streqb    r0,[r8,#3]  @ pBuf[pix] = (colRow | pixels[pix])
        strb      r0,[r7,#3]  @ pPriority[pix] = 1 (in fact non 0)
ppu_renderOAMLinea_check0DrawBehindPix4:
    @pixels[4]
    @ is pixel transparent ?
        tst       r4,#0x03
        beq       ppu_renderOAMLinea_check0DrawBehindPix5
    @ is priority_buf == 0 ?
        tst       r2,#0x03      
    @ draw pixel & update priority_buf                  
        streqb    r4,[r8,#4]  @ pBuf[pix] = (colRow | pixels[pix])
        strb      r4,[r7,#4]  @ pPriority[pix] = 1 (in fact non 0)
ppu_renderOAMLinea_check0DrawBehindPix5:
    @pixels[5]
    @ is pixel transparent ?
        tst       r4,#0x0300
        beq       ppu_renderOAMLinea_check0DrawBehindPix6
    @ is priority_buf == 0 ?
        tst       r2,#0x0300    
    @ draw pixel & update priority_buf          
        mov       r0,r4,lsr #8    
        streqb    r0,[r8,#5]  @ pBuf[pix] = (colRow | pixels[pix])
        strb      r0,[r7,#5]  @ pPriority[pix] = 1 (in fact non 0)
ppu_renderOAMLinea_check0DrawBehindPix6:
    @pixels[6]
    @ is pixel transparent ?
        tst       r4,#0x030000
        beq       ppu_renderOAMLinea_check0DrawBehindPix7
    @ is priority_buf == 0 ?
        tst       r2,#0x030000          
    @ draw pixel & update priority_buf          
        mov       r0,r4,lsr #16
        streqb    r0,[r8,#6]  @ pBuf[pix] = (colRow | pixels[pix])
        strb      r0,[r7,#6]  @ pPriority[pix] = 1 (in fact non 0)
ppu_renderOAMLinea_check0DrawBehindPix7:
    @pixels[7]
    @ is pixel transparent ?
        tst       r4,#0x03000000
        beq       ppu_renderOAMLinea_check0DrawBehindPixEnd
    @ is priority_buf == 0 ?
        tst       r2,#0x03000000        
    @ draw pixel & update priority_buf          
        mov       r0,r4,lsr #24
        streqb    r0,[r8,#7]  @ pBuf[pix] = (colRow | pixels[pix])
        strb      r0,[r7,#7]  @ pPriority[pix] = 1 (in fact non 0)
ppu_renderOAMLinea_check0DrawBehindPixEnd:
                    
ppu_renderOAMLinea_check0NextSprite:

  @next sprite  
        add       r10,r10,#4
  @is it the last ?
        subs      r11,r11,#1
        bne       ppu_renderOAMLinea_check0
                                
ppu_renderOAMLineaCheckClip:
 @end, check if we have to clip 
        ldr       r0, = ppuRegs
        ldrb      r1,[r0,#REG_CTRL1]
        tst       r1,#REGF_CTRL1_OBJNOCLIP
        bne       DontClip2
@       ldmeqfd   r13!,{r1-r2}  
@       streq     r1,[r12,#8]
@       streq     r2,[r12,#12]  
        ldr        r6, = ppuTileXOfs
        ldr        r6,[r6]
        add        r6,r6,r12
        ldmfd     r13!,{r1-r2}  
        strb      r1,[r6,#8]
        mov               r1,r1,lsr #8
        strb      r1,[r6,#9]
        mov               r1,r1,lsr #8
        strb      r1,[r6,#10]
        mov               r1,r1,lsr #8
        strb      r1,[r6,#11]
        mov               r1,r1,lsr #8
        strb      r2,[r6,#12]   
        mov               r2,r2,lsr #8
        strb      r2,[r6,#13]   
        mov               r2,r2,lsr #8
        strb      r2,[r6,#14]   
        mov               r2,r2,lsr #8
        strb      r2,[r6,#15]   
        mov               r2,r2,lsr #8
DontClip2:
        
        
        ldr       r0, = sprite0hit
        ldrb      r1,[r0]
        tst       r1,#0xFF
        ldrne     r0, = ppu_sprite0
        movne     r1,#1
        strne     r1,[r0]
        ldrne     r0, = ppu_StrikeCycle
        movne     r1,#0
        strne     r1,[r0]
        
end_ppu_renderOAMLinea:
        ldmfd     r13!,{r4-r11}
        @bx       lr            
        mov           pc,lr


@priority buffer
@ filled with non-zero (1) when sprite pixel drawn
priority_buf: .fill 272,1,0
sprite0hit:   .long 0 
	
	.pool
