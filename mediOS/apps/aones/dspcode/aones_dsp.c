#include <stdio.h>
#include <string.h>

#include <csl.h>
#include <csl_mcbsp.h>
#include <csl_irq.h>
#include <csl_dma.h>

#include "ibdma.h"

#include "../dspshared.h"
#include "../nes_pal.h"

#define CHUNK_HEIGHT 15

//#define IDMA

unsigned short inBuf[NES_BUFFER_WIDTH*CHUNK_HEIGHT];

unsigned long outBuf[NES_WIDTH*CHUNK_HEIGHT];
unsigned long outBuf2[NES_WIDTH*CHUNK_HEIGHT];

unsigned long framePal[32];

ioport unsigned short portFFFF;
tDspCom dspComBuffer;
tDspCom * dspCom;

#ifdef IDMA
DMA_Config iDmaCfg = {
    1 ,
    DMA_DMMCR_RMK(
		DMA_DMMCR_AUTOINIT_OFF,
		DMA_DMMCR_DINM_OFF,
		DMA_DMMCR_IMOD_FULL_ONLY,
		DMA_DMMCR_CTMOD_MULTIFRAME,
		DMA_DMMCR_SLAXS_ON,
		DMA_DMMCR_SIND_POSTINC,
		DMA_DMMCR_DMS_DATA,
		DMA_DMMCR_DLAXS_OFF,
		DMA_DMMCR_DIND_POSTINC,
		DMA_DMMCR_DMD_DATA
	),  
	DMA_DMSFC_RMK(
		DMA_DMSFC_DSYN_NONE,
		DMA_DMSFC_DBLW_OFF,
		DMA_DMSFC_FRAMECNT_OF(0)
	),
    (DMA_AdrPtr)IB_BUFA_ADDR, // source 
    (DMA_AdrPtr)&inBuf, // dest
    sizeof(inBuf)
};
#endif

// direction : 1 dsp-->sdram, 0 sdram-->dsp
int dma_dsp2sdram(void * dsp_addr, unsigned long sdram_addr,unsigned short length, short direction);
int dma_pending();

void debug(const char* msg);
void interruptARM();

void main(){
	short i,j,k;
#ifdef IDMA
	DMA_Handle iDma;
#endif

    *DSP_COM=0;

	CSL_init();
	ibdma_reset();

#ifdef IDMA
	iDma=DMA_open(DMA_CHA0,DMA_OPEN_RESET);
	if(iDma==INV) return;
#endif

    dspCom = &dspComBuffer;
	memset((void *)dspCom,0,sizeof(dspCom));
    *DSP_COM = (unsigned short) dspCom;

	// swap cb and cr
	for(i=0;i<64;++i){
		nes_pal[i]= (nes_pal[i]/0x10000) + (nes_pal[i]*0x10000);
	}

	debug("DSP START");

	for(;;){
		if(dspCom->inBufReady){
			unsigned long inSdAddr=dspCom->inBufAddr - SDRAM_OFFSET;
			unsigned long outSdAddr=dspCom->outBufAddr - SDRAM_OFFSET;
			int outNum=0;
			int line=0;

//		    interruptARM();

			for(i=0;i<32;++i){
			    framePal[i]=nes_pal[dspCom->pal[i]];
			}

			for(i=0;i<NES_PAL_HEIGHT/CHUNK_HEIGHT;++i){
				unsigned short * inPtr=inBuf;
				unsigned long * outPtr;

				if (outNum==0){
					outPtr=outBuf;
				}else{
					outPtr=outBuf2;
				}

				ibdma_start(inSdAddr,NES_BUFFER_WIDTH*CHUNK_HEIGHT,
							0,NES_BUFFER_WIDTH*CHUNK_HEIGHT,
							NES_BUFFER_WIDTH*CHUNK_HEIGHT,1,
							IB_BUF_A,IB_DIR_SD2IB,1);
				while(ibdma_pending());
				ibdma_reset();
#ifndef IDMA
				memcpy(inBuf,IB_BUFA_ADDR,sizeof(inBuf));
#else
				DMA_config(iDma,&iDmaCfg);
				DMA_start(iDma);
#endif

				for(j=0;j<CHUNK_HEIGHT;++j){
					inPtr=inBuf+j*NES_BUFFER_WIDTH+dspCom->lineOffset[line];

					for (k=0;k<256;k++){
						*outPtr++=framePal[*inPtr++];
					}
					line++;
				}

#ifdef IDMA
				while(DMA_getStatus(iDma));               
#endif

				while(dma_pending());
				if (outNum==0){
					dma_dsp2sdram(outBuf,outSdAddr,sizeof(outBuf)*2,1);
					outNum++;
				}else{
					dma_dsp2sdram(outBuf2,outSdAddr,sizeof(outBuf2)*2,1);
					outNum--;
				}

				inSdAddr+=(unsigned long) NES_BUFFER_WIDTH*CHUNK_HEIGHT;
				outSdAddr+=(unsigned long) NES_WIDTH*CHUNK_HEIGHT*4;
			}
			dspCom->inBufReady=0;
			dspCom->outBufReady=1;
		    interruptARM();
		}
	}
}

void debug(const char* msg){
	strcpy((char*)dspCom->dbgMsg,msg);
	dspCom->hasDbgMsg=1;

    interruptARM();

	while(dspCom->hasDbgMsg);
}

void interruptARM(){
	// Interrupt ARM
	portFFFF |= 0x0001;

	// Clear interrupt ARM
	portFFFF &= 0xFFFE;
}
