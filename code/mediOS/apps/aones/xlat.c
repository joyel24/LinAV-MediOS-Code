#include "medios.h"
#include "aones.h"
#include "datatypes.h"
#include "unes_ppu.h"
#include "nes_pal.h"

extern uint16 *lj_curRenderingScreenPtr;

void XlatNESBufferLine (unsigned char *src, unsigned char *dest,uint8 *nesppupal, uint32 scanl)
{
  return;
/*	uint8 *source = (uint8*)(src);//Vnes.var.Vbuffer + scanl*(256+16)+8);
	uint16 *destin = lj_curRenderingScreenPtr+scanl*(256);
	int j;
	for (j=0;j<256;j++)
	{
		*destin++=scalePal[nesppupal[*source++]];
	}*/
}

#ifndef SCREEN_USE_DSP
__IRAM_CODE
#endif
void XlatNESBufferLineLazy (unsigned char *src, unsigned char *dest,uint8 *nesppupal, uint32 startscanl)
{
# if 0

	int i,j;
  unsigned long *ldest=dest;
  unsigned long pal[32];

  for(i=0;i<32;++i){
    pal[i]=nes_pal[nesppupal[i]];
  }

	for (i=ppu_currentscanline()-startscanl;i;i--,src+=16)
    for (j=0;j<32;j++)
		{
			*ldest++=pal[*src++];
			*ldest++=pal[*src++];
			*ldest++=pal[*src++];
			*ldest++=pal[*src++];
			*ldest++=pal[*src++];
			*ldest++=pal[*src++];
			*ldest++=pal[*src++];
			*ldest++=pal[*src++];
		}

#else

    asm("                                                                    \n\
        stmfd sp!,{r0-r12,lr}                                                \n\
                                                                             \n\
        mov r4,#32                                                           \n\
        add r2,r2,#31                                                        \n\
        ldr r5,=nes_pal                                                      \n\
        sub sp,sp,#4                                                         \n\
                                                                             \n\
        pal_loop:                                                            \n\
            ldrb r6,[r2],#-1                                                 \n\
            ldr r6,[r5,r6,lsl #2]                                            \n\
            str r6,[sp],#-4                                                  \n\
            subs r4,r4,#1                                                    \n\
            bhi pal_loop                                                     \n\
                                                                             \n\
        add sp,sp,#4                                                         \n\
                                                                             \n\
        ldr r2,=ppuScanline                                                  \n\
        ldr r2,[r2]                                                          \n\
        sub r3,r2,r3                                                         \n\
                                                                             \n\
        mov r12,#0x3fc                                                       \n\
                                                                             \n\
        sub r0,r0,#16                                                        \n\
                                                                             \n\
        line_loop:                                                           \n\
            add r0,r0,#16                                                    \n\
            mov r2,#32                                                       \n\
                                                                             \n\
            pxl_loop:                                                        \n\
/*                ldmia r0!,{r4,r8}                                            \n\
                                                                             \n\
@                ldr r4,[r0],#4                                               \n\
@                ldr r8,[r0],#4                                               \n\
                                                                             \n\
                and r5,r12,r4,lsr #6                                         \n\
                and r6,r12,r4,lsr #14                                        \n\
                and r7,r12,r4,lsr #22                                        \n\
                and r4,r12,r4,lsl #2                                         \n\
                                                                             \n\
                and r9,r12,r8,lsr #6                                         \n\
                and r10,r12,r8,lsr #14                                       \n\
                and r11,r12,r8,lsr #22                                       \n\
                and r8,r12,r8,lsl #2                                         \n\
                                                                             \n\
                ldr r4,[sp,r4]                                               \n\
                ldr r5,[sp,r5]                                               \n\
                ldr r6,[sp,r6]                                               \n\
                ldr r7,[sp,r7]                                               \n\
                                                                             \n\
                ldr r8,[sp,r8]                                               \n\
                ldr r9,[sp,r9]                                               \n\
                ldr r10,[sp,r10]                                             \n\
                ldr r11,[sp,r11]                                             \n\
                                                                             \n\
*/                                                                           \n\
                ldrb r4,[r0],#1                                              \n\
                ldrb r5,[r0],#1                                              \n\
                ldrb r6,[r0],#1                                              \n\
                ldrb r7,[r0],#1                                              \n\
                ldrb r8,[r0],#1                                              \n\
                ldrb r9,[r0],#1                                              \n\
                ldrb r10,[r0],#1                                             \n\
                ldrb r11,[r0],#1                                             \n\
                                                                             \n\
                ldr r4,[sp,r4,lsl #2]                                        \n\
                ldr r5,[sp,r5,lsl #2]                                        \n\
                ldr r6,[sp,r6,lsl #2]                                        \n\
                ldr r7,[sp,r7,lsl #2]                                        \n\
                                                                             \n\
                ldr r8,[sp,r8,lsl #2]                                        \n\
                ldr r9,[sp,r9,lsl #2]                                        \n\
                ldr r10,[sp,r10,lsl #2]                                      \n\
                ldr r11,[sp,r11,lsl #2]                                      \n\
                                                                             \n\
                stmia r1!,{r4,r5,r6,r7,r8,r9,r10,r11}                        \n\
                                                                             \n\
                subs r2,r2,#1                                                \n\
                bhi pxl_loop                                                 \n\
                                                                             \n\
            subs r3,r3,#1                                                    \n\
            bhi line_loop                                                    \n\
                                                                             \n\
        add sp,sp,#128                                                       \n\
    		ldmfd sp!,{r0-r12,pc}                                                \n\
    ");
    
#endif

}
