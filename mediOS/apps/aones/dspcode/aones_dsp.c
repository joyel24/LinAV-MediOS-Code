#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <csl.h>
#include <csl_mcbsp.h>
#include <csl_irq.h>
#include <csl_dma.h>

#include "ibdma.h"

#include "../dspshared.h"
#include "../nes_pal.h"
#include "nes_apu.h"

// seems to be the best values for speed and sound quality
#define CHUNK_HEIGHT 5
#define SNDBUF_LENGTH 20

//#define IDMA

unsigned short sndBuf[SNDBUF_LENGTH];
unsigned short sndBuf2[SNDBUF_LENGTH];
unsigned short sndBufNum;

unsigned short inBuf[NES_BUFFER_WIDTH*CHUNK_HEIGHT];

unsigned long outBuf[NES_WIDTH*CHUNK_HEIGHT];
unsigned long outBuf2[NES_WIDTH*CHUNK_HEIGHT];

unsigned long framePal[32];

ioport unsigned short portFFFF;
tDspCom dspComBuffer;
tDspCom * dspCom;

Uint16 aicDmaEventID;

extern void VECSTART(void);

DMA_Handle aicDma;

#ifdef IDMA
DMA_Handle iDma;

DMA_Config iDmaCfg = {
    2 ,
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

static MCBSP_Handle aicDataPort;

MCBSP_Config aicDataPortCfg= {
  0x0000,0x0200, /* SPCR : free running mode */
  0x00A0,0x00A1, /* RCR  : 32 bit receive data length */  
  0x00A0,0x00A0, /* XCR  : 32 bit transmit data length */            
  0x0000,0x3000, /* SRGR 1 & 2 */
  0x0000,0x0000, /* MCR  : single channel */
  // VP : frame transmit polarity was wrong (bit 3 has to be cleared)
  0x000E - 8,        /* PCR  : FSX, FSR active low, external FS/CLK source */
  0x0000,
  0x0000,
  0x0000,
  0x0000
};  

// direction : 1 dsp-->sdram, 0 sdram-->dsp
int dma_dsp2sdram(void * dsp_addr, unsigned long sdram_addr,unsigned short length, short direction);
int dma_pending();

void debug(const char* msg);
void interruptARM();
void handleVideoBuffer();
void doAicDma(unsigned short * buffer,unsigned short length);
interrupt void aicDmaEnd(void);

void main(){
	short i;

    *DSP_COM=0;

	CSL_init();
	ibdma_reset();

	// dmas
 	aicDma = DMA_open(DMA_CHA3, DMA_OPEN_RESET);    
    aicDmaEventID = DMA_getEventId(aicDma);      
    DMA_FSET(DMPREC,INTOSEL,DMA_DMPREC_INTOSEL_CH2_CH3);

#ifdef IDMA
	iDma=DMA_open(DMA_CHA0,DMA_OPEN_RESET);
	if(iDma==INV) return;
#endif

	// aic23
	aicDataPort = MCBSP_open(MCBSP_PORT0, MCBSP_OPEN_RESET);
	MCBSP_config(aicDataPort,&aicDataPortCfg);
	MCBSP_start(aicDataPort,MCBSP_XMIT_START,0);

 	// irqs
    IRQ_globalDisable();
    IRQ_plug(aicDmaEventID,&aicDmaEnd);
	IRQ_clear(aicDmaEventID);
    IRQ_enable(aicDmaEventID);
    IRQ_globalEnable();
    
    // nes apu
 	apu_create(SAMPLE_RATE, 60, 0, 16);
    apu_setfilter(APU_FILTER_NONE);

	// launch dma
	sndBufNum=0;
	memset(sndBuf,0,SNDBUF_LENGTH);
	doAicDma(sndBuf,SNDBUF_LENGTH);

	// palette : swap cb and cr
	for(i=0;i<64;++i){
		nes_pal[i]= (nes_pal[i]/0x10000) + (nes_pal[i]*0x10000);
	}

    // dsp<>arm comm
    dspCom = &dspComBuffer;
	memset((void *)dspCom,0,sizeof(dspCom));
    *DSP_COM = (unsigned short) dspCom;

	if (aicDma==INV) debug("AIC DMA NOK");
	if (aicDataPort==INV) debug("Data Port NOK");

	debug("DSP START");

	// the main prog handles video, sound is done with dma+irq
	for(;;){
		if(dspCom->inBufReady){
#if 0
			dspCom->inBufReady=0;
#else
		    interruptARM();
			handleVideoBuffer();
#endif
		}
	}
}

void handleVideoBuffer(){
	unsigned long inSdAddr=dspCom->inBufAddr - SDRAM_OFFSET;
	unsigned long outSdAddr=dspCom->outBufAddr - SDRAM_OFFSET;
	unsigned short * inPtr;
	unsigned long * outPtr;
	short outBufNum=0;
	short line=0;
	short i,j,k;

	// build frame palette
	for(i=0;i<32;++i){
	    framePal[i]=nes_pal[dspCom->pal[i]];
	}

	line+=10;
	inSdAddr+=(unsigned long) NES_BUFFER_WIDTH*10;
	outSdAddr+=(unsigned long) NES_WIDTH*10*4;

	for(i=0;i<(NES_PAL_HEIGHT-20)/CHUNK_HEIGHT;++i){
		inPtr=inBuf;

		if (outBufNum==0){
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
		if (outBufNum==0){
			dma_dsp2sdram(outBuf,outSdAddr,sizeof(outBuf)*2,1);
			outBufNum++;
		}else{
			dma_dsp2sdram(outBuf2,outSdAddr,sizeof(outBuf2)*2,1);
			outBufNum--;
		}

		inSdAddr+=(unsigned long) NES_BUFFER_WIDTH*CHUNK_HEIGHT;
		outSdAddr+=(unsigned long) NES_WIDTH*CHUNK_HEIGHT*4;
	}
	dspCom->inBufReady=0;
	dspCom->outBufReady=1;
    interruptARM();
}

void doAicDma(unsigned short * buffer,unsigned short length){

    /* Write configuration structure values to DMA control regs */ 
    DMA_configArgs(
	   aicDma,    
	   1 ,                                  /* Priority */
	   DMA_DMMCR_RMK(
			 DMA_DMMCR_AUTOINIT_OFF,
			 DMA_DMMCR_DINM_ON,
			 DMA_DMMCR_IMOD_FULL_ONLY,
			 0, /* not ABU */
			 DMA_DMMCR_SLAXS_OFF,
			 DMA_DMMCR_SIND_POSTINC,
			 DMA_DMMCR_DMS_DATA,
			 DMA_DMMCR_DLAXS_OFF,
			 DMA_DMMCR_DIND_NOMOD,
			 DMA_DMMCR_DMD_DATA
			 ),                             /* DMMCR */
	   DMA_DMSFC_RMK(
			 DMA_DMSFC_DSYN_XEVT0,
			 DMA_DMSFC_DBLW_ON,
			 DMA_DMSFC_FRAMECNT_OF(0)
			 ),                             /* DMSFC */ 
	   (DMA_AdrPtr)(buffer+2),              /* DMSRC */
	   (DMA_AdrPtr)MCBSP_ADDR(DXR20),       /* DMDST */
	   (Uint16)(length-2)/2-1               /* DMCTR = buffsize */
	);

    while(!MCBSP_xrdy(aicDataPort));
    MCBSP_write32(aicDataPort,*(long *)buffer);

    /* Start DMA transfer */
    DMA_start(aicDma);
}

interrupt void aicDmaEnd(void){
	// send first buffer to aic
	if (sndBufNum==0){
		doAicDma(sndBuf,SNDBUF_LENGTH);
		sndBufNum++;
	}else{
		doAicDma(sndBuf2,SNDBUF_LENGTH);
		sndBufNum--;
	}

	// handle reset
	if (dspCom->sndWantApuReset){
		debug("apu reset");
		apu_reset();
		dspCom->sndWantApuReset=0;
	}

	dspCom->sndStatusReg=apu_read(APU_SMASK);

	// render second buffer
	if (sndBufNum==0){
		apu_process(sndBuf,SNDBUF_LENGTH/2);    	
	}else{
		apu_process(sndBuf2,SNDBUF_LENGTH/2);    	
	}
}

void debug(const char* msg){
	if (!dspCom) return;

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
